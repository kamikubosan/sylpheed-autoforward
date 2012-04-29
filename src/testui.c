#include <gtk/gtk.h>

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    g_print ("delete event occurred\n");

    return FALSE;
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

static void add_folder( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

static void forward_all( GtkButton *widget,
                     gpointer   data )
{
  gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
  
}

static void forward_folder( GtkButton *widget,
                     gpointer   data )
{
  gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
}

static void delete_folder( GtkWidget *widget,
                     gpointer   data )
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW(data) );
   GtkListStore *store;
   GtkTreeModel *model;
   GtkTreeIter iter;
   
   if (gtk_tree_selection_count_selected_rows(selection) == 0){
     g_print("no selection\n");
      return;
   }
   GList *list = gtk_tree_selection_get_selected_rows( selection, &model );
   store = GTK_LIST_STORE ( model );
   
   if (!list){
     g_print("no list\n");
     return;
   }

   int nRemoved = 0;
   while(list) {
      int ipath = atoi(gtk_tree_path_to_string(list->data));
      ipath-=nRemoved;
      GString *fixed_path = g_string_new("");
      g_string_printf(fixed_path, "%d", ipath);
      
      GtkTreePath *path = gtk_tree_path_new_from_string(fixed_path->str);
      g_string_free(fixed_path, TRUE);
      
      if (path) {
         if ( gtk_tree_model_get_iter ( model, &iter, path) ) { 
            gtk_list_store_remove ( store, &iter );
            nRemoved++;   
         }
         gtk_tree_path_free (path);
      }
      list = list->next;
   }
   g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);

   g_list_free (list);
}
int main(int argc, char* argv[])
{
  GtkWidget *window, *radio1, *radio2, *box, *entry; 

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect (window, "delete-event",
                    G_CALLBACK (delete_event), NULL);
    
  g_signal_connect (window, "destroy",
                    G_CALLBACK (destroy), NULL);

  box = gtk_vbox_new (FALSE, 2);
  /* Create a radio button with a GtkEntry widget */
   radio1 = gtk_radio_button_new_with_label(NULL, "Forward all mail");
   /* Create a radio button with a label */
   radio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio1),
                                                         "Forward mail in folder");

   
   /* Pack them into a box, then show all the widgets */
   gtk_box_pack_start (GTK_BOX (box), radio1, FALSE, FALSE, 2);
   gtk_box_pack_start (GTK_BOX (box), radio2, FALSE, FALSE, 2);

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
   
   /**/
    GtkWidget *bbox = gtk_hbutton_box_new();
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(bbox), 6);
    GtkWidget *btn_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
    GtkWidget *btn_delete = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    GtkWidget *btn_exit = gtk_button_new_from_stock(GTK_STOCK_QUIT);

    g_signal_connect (btn_exit, "clicked",
		      G_CALLBACK (destroy), NULL);
       
    g_signal_connect (btn_add, "clicked",
		      G_CALLBACK (add_folder), view);

    g_signal_connect (btn_delete, "clicked",
		      G_CALLBACK (delete_folder), view);

    g_signal_connect(GTK_BUTTON(radio1), "clicked",
                     G_CALLBACK(forward_all), view);

    g_signal_connect(GTK_BUTTON(radio2), "clicked",
                     G_CALLBACK(forward_folder), view);

      gtk_box_pack_start(GTK_BOX(bbox), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bbox), btn_delete, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bbox), btn_exit, FALSE, FALSE, 0);
    gtk_widget_show_all(bbox);
    
    gtk_box_pack_end(GTK_BOX(box), bbox, FALSE, FALSE, 6);

   gtk_container_add (GTK_CONTAINER (window), box);
   gtk_widget_show_all (window);

   gtk_main ();
   return;
}
