#include <gtk/gtk.h>

int main(int argc, char* argv[])
{
    GtkWidget *window, *radio1, *radio2, *box, *entry; 

     gtk_init (&argc, &argv);

     window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   box = gtk_vbox_new (TRUE, 2);
   /* Create a radio button with a GtkEntry widget */
   radio1 = gtk_radio_button_new_with_label(NULL, "Forward all mail");
   /* Create a radio button with a label */
   radio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio1),
                                                         "Forward mail in folder");
   /* Pack them into a box, then show all the widgets */
   gtk_box_pack_start (GTK_BOX (box), radio1, TRUE, TRUE, 2);
   gtk_box_pack_start (GTK_BOX (box), radio2, TRUE, TRUE, 2);

   /**/
   GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);
   GtkTreeIter iter;
   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "foo", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

      gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes ("Folder",
                                                    gtk_cell_renderer_text_new (),
                                                    "text", 0,
                                                    NULL);
   GtkWidget *view=gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
   gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
   GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_container_add (GTK_CONTAINER (sw), view);
   gtk_box_pack_start (GTK_BOX (box), sw, TRUE, TRUE, 2);
   
   
   gtk_container_add (GTK_CONTAINER (window), box);
   gtk_widget_show_all (window);

   gtk_main ();
   return;
}
