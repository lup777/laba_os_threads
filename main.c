// main.c

#include <pthread.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

#define NUM_OF_THREADS 10

#define bool int
#define true 1
#define false 0

void create_thread(int num, int wait);
static gboolean set_text_buffer(void *data);

struct DispatchData {
    GtkTextBuffer *buffer;
    char* output_str;
    string str;    
};

class CONTEXT
{
 private:
    pthread_t threads[NUM_OF_THREADS];
    string txt;
    pthread_mutex_t thread_mutex;
    pthread_mutex_t txt_mutex;
    pthread_mutex_t buffer_mutex;
    pthread_mutex_t print_mutex;
    GtkTextBuffer *buffer;

 public:

  CONTEXT()
      {
          pthread_mutex_init( &thread_mutex, NULL );
          pthread_mutex_init( &txt_mutex, NULL );
          pthread_mutex_init( &buffer_mutex, NULL );
          pthread_mutex_init( &print_mutex, NULL );
          buffer = NULL;
      }

  void add_line(string str)
  {
      pthread_mutex_lock(&print_mutex);
      string tmp_txt = get_txt();
      tmp_txt += str + string("\n");

      //gtk_text_buffer_set_text (context.buffer, tmp_txt.c_str(), tmp_txt.length());
      //set_text_buffer(tmp_txt);
      struct DispatchData *data = g_new0(struct DispatchData, 1);
      data->output_str = g_strdup_printf("%s", tmp_txt.c_str());
      data->buffer = buffer;
      gdk_threads_add_idle(set_text_buffer, data);

      set_txt(tmp_txt);

      cout << tmp_txt.c_str() << endl;
      pthread_mutex_unlock(&print_mutex);
  }


  string get_txt(void)
  {
      pthread_mutex_lock(&txt_mutex);
      string tmp = txt;
      pthread_mutex_unlock(&txt_mutex);
      return txt;
  }

  void set_txt(string new_txt)
  {
      pthread_mutex_lock(&txt_mutex);
      txt = new_txt;
      pthread_mutex_unlock(&txt_mutex);
  }
  
  pthread_t get_tid(int num)
    {
        pthread_mutex_lock(&thread_mutex);
        pthread_t tmp = threads[num];
        pthread_mutex_unlock(&thread_mutex);
        return tmp;
    }

  void set_tid(int num, pthread_t pid)
    {
        pthread_mutex_lock(&thread_mutex);
        threads[num] = pid;
        pthread_mutex_unlock(&thread_mutex);
    }

  void set_buffer_pointer(GtkTextBuffer *buffer_)
  {
      pthread_mutex_lock(&buffer_mutex);
      buffer = buffer_;
      pthread_mutex_unlock(&buffer_mutex);
  }
  
  void init_text_buffer(void)
  {
      pthread_mutex_lock(&buffer_mutex);
      //buffer = gtk_text_buffer_new(NULL);
      pthread_mutex_unlock(&buffer_mutex);
  }
  

};



//static gboolean set_text_buffer(struct DispatchData *data)
static gboolean set_text_buffer(void *vdata)
{
    struct DispatchData *data = (struct DispatchData *)vdata;
    gtk_text_buffer_set_text (data->buffer, data->output_str, strlen(data->output_str));
    g_free(data);
    return G_SOURCE_REMOVE;
}


CONTEXT context;

void *thread_x(void *param)
{
    int* p_num = (int*)param;
    int num = *p_num;
    char name = 'A';
    switch(num)
        {
        case 1: name = 'A'; break;
        case 2: name = 'B'; break;
        case 3: name = 'C'; break;
        case 4: name = 'D'; break;
        case 5: name = 'E'; break;
        case 6: name = 'F'; break;
        case 7: name = 'G'; break;
        case 8: name = 'H'; break;
        case 9: name = 'K'; break;
        default:
            printf("\nFATAL ERROR !\n\n");
            exit(1);
        }
  

    {
        stringstream tmp;
        tmp << "thread " << name << " >>>";
        context.add_line(tmp.str());
    }
    
    sleep(1);

    {
        stringstream tmp;
        tmp << "thread " << name << " <<<";
        context.add_line(tmp.str());
    }

}

/*case 1: name = 'A'; break;
        case 2: name = 'B'; break;
        case 3: name = 'C'; break;
        case 4: name = 'D'; break;
        case 5: name = 'E'; break;
        case 6: name = 'F'; break;
        case 7: name = 'G'; break;
        case 8: name = 'H'; break;
        case 9: name = 'K'; break;*/

void* work(void* param)
{
    create_thread(1, true); //A

    create_thread(2, false); // B
    create_thread(3, false); // C

    pthread_join(context.get_tid(2), NULL); // B

    create_thread(4, false); // D
    create_thread(5, false); // E
    create_thread(6, false); // F
    pthread_join(context.get_tid(4), NULL); // D
    pthread_join(context.get_tid(5), NULL); // E
    pthread_join(context.get_tid(6), NULL); // F

    create_thread(7, false); // G
    create_thread(8, false); // H
    pthread_join(context.get_tid(7), NULL); // G
    pthread_join(context.get_tid(8), NULL); // H
    pthread_join(context.get_tid(3), NULL); // C

    create_thread(9, true); // K
  
    cout << "the end" << endl;
}

void create_thread(int num, int wait)
{
  pthread_attr_t attr;
  pthread_t tid;
  pthread_attr_init( &attr );

  int* p_param = new int;
  *p_param = num;
  pthread_create( &tid, &attr, thread_x, (void*)p_param );

  context.set_tid(num, tid);

  if(wait == true)
    { //wait for thread ends
      pthread_join(tid, NULL);
    }
}

static void
activate (GtkApplication* app,
	  gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  //gtk_widget_show_all (window);

  //text widget

  view = gtk_text_view_new ();

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  context.set_buffer_pointer(buffer);

  /* Change left margin throughout the widget */
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (view), 30);


  //gtk_container_add (GTK_CONTAINER (window), view);

  //BOX
  GtkWidget* box1 = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), box1);

  
  
  // IMAGE
  //GtkWidget * img_widget = gtk_image_new();
  
  GtkWidget * img_widget = gtk_image_new_from_file("022vWqAz0gk.jpg");
  
  //gtk_container_add (GTK_CONTAINER (window), img_widget);
  gtk_widget_queue_draw(img_widget);
  //IMAGE

  gtk_box_pack_start (GTK_BOX(box1), img_widget, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX (box1), view, TRUE, TRUE, 0);

  gtk_widget_show_all (window);
  cout << "started " << endl;
  {
      pthread_t tid;
      pthread_attr_t attr;
      pthread_attr_init( &attr );
      
      pthread_create( &tid, &attr, work, (void*)NULL );
  }

}

int main(int argc, char **argv)
{
  GtkApplication *app;
  int status;
  

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  printf("GoodBy!\n");
  return status;
}
