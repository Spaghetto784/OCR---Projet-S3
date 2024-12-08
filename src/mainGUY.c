#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "../include/image_processing.h"
#include "../include/load_image.h"
#include "../include/detect.h"

SDL_Surface *current_image = NULL;
GtkWidget *image_widget = NULL;

typedef struct Node {
    SDL_Surface *image;
    struct Node *next;
} Node;

Node *undo_stack = NULL;

GtkWidget* create_icon_button(const char* label_text, const char* icon_path, int width, int height) 
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(icon_path, NULL);
    if (!pixbuf) 
    {
        g_print("Erreur : Impossible de charger l'icône : %s\n", icon_path);
        pixbuf = gdk_pixbuf_new_from_file("src/icons/default.png", NULL);
    }
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);
    GtkWidget *icon = gtk_image_new_from_pixbuf(scaled_pixbuf);
    g_object_unref(scaled_pixbuf);
    GtkWidget *label = gtk_label_new(label_text);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(button), box);
    return button;
}


void apply_css(GtkWidget *widget __attribute__((unused)), const char *css_file) 
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, css_file, &error);
    if (error) {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_error_free(error);
    }
    g_object_unref(provider);
}

void push_undo_stack(SDL_Surface *image) 
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) 
    {
        return;
    }
    new_node->image = SDL_ConvertSurface(image, image->format, 0);
    if (!new_node->image) 
    {
        free(new_node);
        return;
    }
    new_node->next = undo_stack;
    undo_stack = new_node;
}

SDL_Surface *pop_undo_stack() 
{
    if (!undo_stack) 
    {
        return NULL;
    }
    Node *top = undo_stack;
    SDL_Surface *image = top->image;
    undo_stack = top->next;
    free(top);
    return image;
}

void clear_undo_stack() 
{
    while (undo_stack) 
    {
        Node *top = undo_stack;
        SDL_FreeSurface(top->image);
        undo_stack = top->next;
        free(top);
    }
}

void save_current_state() 
{
    if (current_image) 
    {
        push_undo_stack(current_image);
    }
}

GdkPixbuf* sdl_surface_to_pixbuf(SDL_Surface *surface) 
{
    if (!surface)
    {
        return NULL;
    }
    int width = surface->w, height = surface->h, channels = surface->format->BytesPerPixel;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(
        (guchar *)surface->pixels, GDK_COLORSPACE_RGB, channels == 4, 8,
        width, height, surface->pitch, NULL, NULL);
    return pixbuf;
}

void resize_window_to_image(GtkWidget *window, SDL_Surface *image) 
{
    if (!image) 
    {
        return;
    }
    int image_width = image->w;
    int image_height = image->h;
    int min_width = 1200;
    int min_height = 800;
    int window_width = (image_width > min_width) ? image_width : min_width;
    int window_height = (image_height > min_height) ? image_height : min_height;
    gtk_window_resize(GTK_WINDOW(window), window_width, window_height);
}

void update_image_widget() 
{
    if (current_image) 
    {
        GdkPixbuf *pixbuf = sdl_surface_to_pixbuf(current_image);
        if (pixbuf) 
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), pixbuf);
            GtkWidget *toplevel = gtk_widget_get_toplevel(image_widget);
            int window_width, window_height;
            gtk_window_get_size(GTK_WINDOW(toplevel), &window_width, &window_height);
            int button_box_width = gtk_widget_get_allocated_width(gtk_widget_get_parent(image_widget));
            int content_width = window_width - button_box_width;
            int image_width = current_image->w;
            int image_height = current_image->h;
            int margin_x = (content_width > image_width) ? (content_width - image_width) / 2 : 0;
            int margin_y = (window_height > image_height) ? (window_height - image_height) / 2 : 0;
            gtk_widget_set_margin_start(image_widget, margin_x);
            gtk_widget_set_margin_end(image_widget, margin_x);
            gtk_widget_set_margin_top(image_widget, margin_y);
            gtk_widget_set_margin_bottom(image_widget, margin_y);
            g_object_unref(pixbuf);
        }
    } 
    else 
    {
        gtk_image_clear(GTK_IMAGE(image_widget));
    }
}

void on_undo_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) 
{
    SDL_Surface *previous_image = pop_undo_stack();
    if (!previous_image) 
    {
        return;
    }
    if (current_image) 
    {
        SDL_FreeSurface(current_image);
    }
    current_image = previous_image;
    update_image_widget();
}

void on_rotate_clicked(GtkWidget *widget, gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Image rotation", NULL, GTK_DIALOG_MODAL,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *angle_label = gtk_label_new("Enter rotation angle (in degrees) :");
    GtkWidget *angle_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(content_area), angle_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), angle_entry, FALSE, FALSE, 5);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) 
    {
        const char *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
        int angle = atoi(angle_text);
        if (angle != 0) 
        {
            SDL_Surface *rotated_image = rotate_image(current_image, angle);
            if (rotated_image) 
            {
                SDL_FreeSurface(current_image);
                current_image = rotated_image;
                resize_window_to_image(gtk_widget_get_toplevel(widget), current_image);
                update_image_widget();
            } 
        }
    }
    gtk_widget_destroy(dialog);
}

void process_with_mainDet(SDL_Surface *image) 
{
    if (!image) 
    {
        return;
    }
    SDL_Surface *grayscale_image = preprocess_image(image);
    if (!grayscale_image) 
    {
        return;
    }
    Uint8 threshold = 140;
    SDL_Surface *bw_image = convert_to_bw(grayscale_image, threshold);
    SDL_FreeSurface(grayscale_image);
    if (!bw_image) 
    {
        return;
    }
    SDL_Surface *rotated_image = rotate_image(bw_image, 0); 
    SDL_FreeSurface(bw_image);
    if (!rotated_image) 
    {
        return;
    }
    SDL_LockSurface(rotated_image);
    detect(rotated_image); 
    SDL_UnlockSurface(rotated_image);
    SDL_FreeSurface(current_image);
    current_image = rotated_image;
    update_image_widget();
}

void on_process_with_mainDet_clicked(GtkWidget *widget, gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    process_with_mainDet(current_image);
    resize_window_to_image(gtk_widget_get_toplevel(widget), current_image);
}

void on_load_image_clicked(GtkWidget *widget, gpointer data __attribute__((unused))) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open an image",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        SDL_Surface *image = IMG_Load(filename);

        if (image) {
            if (current_image) SDL_FreeSurface(current_image);
            current_image = image;

            resize_window_to_image(gtk_widget_get_toplevel(widget), current_image);
            update_image_widget();
        } else {
            g_print("Erreur de chargement : %s\n", IMG_GetError());
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void on_enhance_contrast_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) {
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Contrast settings", NULL, GTK_DIALOG_MODAL,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *contrast_label = gtk_label_new("Contraste (ex: 5):");
    GtkWidget *contrast_entry = gtk_entry_new();
    GtkWidget *gamma_label = gtk_label_new("Gamma (ex: 1):");
    GtkWidget *gamma_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(content_area), contrast_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), contrast_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), gamma_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), gamma_entry, FALSE, FALSE, 5);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) 
    {
        const char *contrast_text = gtk_entry_get_text(GTK_ENTRY(contrast_entry));
        const char *gamma_text = gtk_entry_get_text(GTK_ENTRY(gamma_entry));
        double contrast = atof(contrast_text);
        double gamma = atof(gamma_text);
        if (contrast <= 0 || gamma < 0) 
        {
            g_print("Valeurs de contraste ou gamma invalides.\n");
        } 
        else 
        {
            SDL_Surface *enhanced_image = enhance_contrast(current_image, contrast, gamma);
            if (enhanced_image) 
            {
                SDL_FreeSurface(current_image);
                current_image = enhanced_image;
                update_image_widget();
            } 
        }
    }
    gtk_widget_destroy(dialog);
}

void on_grayscale_bw_clicked(GtkWidget *widget, gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    SDL_Surface *grayscale_image = preprocess_image(current_image);
    if (grayscale_image) 
    {
        SDL_FreeSurface(current_image);
        current_image = grayscale_image;
        Uint8 threshold = calculate_threshold(current_image);
        SDL_Surface *bw_image = convert_to_bw(current_image, threshold);
        if (bw_image) 
        {
            SDL_FreeSurface(current_image);
            current_image = bw_image;
            resize_window_to_image(gtk_widget_get_toplevel(widget), current_image);
            update_image_widget();
        } 
        
    }
}

void on_noise_reduction_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused)))
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    SDL_Surface *median = apply_median_filter(current_image, 3);
    if (median)
    {
        SDL_FreeSurface(current_image);
        current_image = median;
    }
    else
    {
        return;
    }
    SDL_Surface *gaussian = apply_gaussian_filter(current_image, 3, 1.0F);
    if (gaussian)
    {
        SDL_FreeSurface(current_image);
        current_image = gaussian;
        update_image_widget();
    }
    else
    {
        return;
    }
}

void on_invert_color_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused)))
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    SDL_Surface *invert = invert_colors(current_image);
    if (invert)
    {
        SDL_FreeSurface(current_image);
        current_image = invert;
        update_image_widget();
    }
    else
    {
        return;
    }
}

void on_detect_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();
    if (current_image->format->BytesPerPixel != 4) 
    {
        SDL_Surface *converted_surface = SDL_ConvertSurfaceFormat(current_image, SDL_PIXELFORMAT_RGBA32, 0);
        if (!converted_surface) 
        {
            g_print("Erreur de conversion de format : %s\n", SDL_GetError());
            return;
        }
        SDL_FreeSurface(current_image);
        current_image = converted_surface;
    }
    SDL_LockSurface(current_image);
    detect(current_image);
    SDL_UnlockSurface(current_image);
    update_image_widget();
}

void on_save_image_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save image",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_filter_set_name(filter, "Images (*.png, *.jpg, *.bmp)");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) 
    {
        char *imagename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (IMG_SavePNG(current_image, imagename) == 0) 
        {
            g_print("Image saved successfully : %s\n", imagename);
        } 
        else 
        {
            g_print("Image not saved Error : %s\n", SDL_GetError());
        }
        g_free(imagename);
    }
    gtk_widget_destroy(dialog);
}

void on_clean_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) 
{
    if (current_image) 
    {
        SDL_FreeSurface(current_image);
        current_image = NULL;
    }
    clear_undo_stack();
    update_image_widget();
}

void on_solver_clicked(GtkWidget *widget __attribute__((unused)), gpointer data __attribute__((unused))) 
{
    if (!current_image) 
    {
        return;
    }
    save_current_state();

    SDL_Surface *new_image = resolve(current_image);
    if (new_image) 
    {
        SDL_FreeSurface(current_image);
        current_image = new_image;
        update_image_widget();
        resize_window_to_image(gtk_widget_get_toplevel(widget), current_image);
    } 
}



int main(int argc, char *argv[]) 
{
    gtk_init(&argc, &argv);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        g_print("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "SegFaultLess - OCR");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    
    // Left column for buttons 
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(button_box, "button-box");
    gtk_box_pack_start(GTK_BOX(hbox), button_box, FALSE, FALSE, 0);
    
    GtkWidget *header_label = gtk_label_new("OCR Tools");
    gtk_widget_set_name(header_label, "header-label");
    gtk_box_pack_start(GTK_BOX(button_box), header_label, FALSE, FALSE, 10);

    // Bouton "Load image"
    GtkWidget *load_button = create_icon_button("Load Image", "src/icons/load.png", 16, 16);
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_image_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), load_button, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator, FALSE, FALSE, 5);

    // Bouton "Rotate image"
    GtkWidget *rotate_button = create_icon_button("Rotate Image", "src/icons/rotate.png", 16, 16);
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(on_rotate_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), rotate_button, FALSE, FALSE, 0);

    // Bouton "Enhace contrast"
     GtkWidget *enhance_contrast_button = create_icon_button("Enhance Contrast", "src/icons/contrast.png", 16, 16);
    g_signal_connect(enhance_contrast_button, "clicked", G_CALLBACK(on_enhance_contrast_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), enhance_contrast_button, FALSE, FALSE, 0);

    // Bouton "Grayscale + BW"
    GtkWidget *grayscale_bw_button = create_icon_button("Grayscale + BW", "src/icons/grayscale.png", 16, 16);
    g_signal_connect(grayscale_bw_button, "clicked", G_CALLBACK(on_grayscale_bw_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), grayscale_bw_button, FALSE, FALSE, 0);

    // Bouton "Noise reduction"
    GtkWidget *noise_reduction_button = create_icon_button("Noise Reduction", "src/icons/noise.png", 16, 16);
    g_signal_connect(noise_reduction_button, "clicked", G_CALLBACK(on_noise_reduction_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), noise_reduction_button, FALSE, FALSE, 0);

    // Bouton "Invert Color"
    GtkWidget *invert_color_button = create_icon_button("Invert Color", "src/icons/invert-colors.png", 16, 16);
    g_signal_connect(invert_color_button, "clicked", G_CALLBACK(on_invert_color_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), invert_color_button, FALSE, FALSE, 0);

    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator1, FALSE, FALSE, 5);

    // Bouton "Detecting letters"
    GtkWidget *detect1_button = create_icon_button("Detect Letters", "src/icons/detect.png", 16, 16);
    g_signal_connect(detect1_button, "clicked", G_CALLBACK(on_process_with_mainDet_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), detect1_button, FALSE, FALSE, 0);

    // Bouton "Solver"
    GtkWidget *solver_button = create_icon_button("Solver", "src/icons/solver.png", 16, 16);
    g_signal_connect(solver_button, "clicked", G_CALLBACK(on_solver_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), solver_button, FALSE, FALSE, 0);

    GtkWidget *separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator2, FALSE, FALSE, 5);

    // Bouton "Save Image"
    GtkWidget *save_button = create_icon_button("Save Image", "src/icons/save.png", 16, 16);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_image_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), save_button, FALSE, FALSE, 0);

    // Bouton "Undo"
    GtkWidget *undo_button = create_icon_button("Undo", "src/icons/undo.png", 16, 16);
    g_signal_connect(undo_button, "clicked", G_CALLBACK(on_undo_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), undo_button, FALSE, FALSE, 0);

    // Bouton "Clean"
    GtkWidget *clean_button = create_icon_button("Clean", "src/icons/clean.png", 16, 16);
    g_signal_connect(clean_button, "clicked", G_CALLBACK(on_clean_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), clean_button, FALSE, FALSE, 0);

    // Image display area
    image_widget = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox), image_widget, TRUE, TRUE, 0);

    apply_css(window, "style.css");

    gtk_widget_show_all(window);
    gtk_main();

    if (current_image)
    {
        SDL_FreeSurface(current_image);
    }
    clear_undo_stack();
    SDL_Quit();
    return 0;
}


