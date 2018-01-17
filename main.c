// main.c

#include <pthread.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string>
#include <sstream>

using namespace std;

#define NUM_OF_THREADS 10

#define bool int
#define true 1
#define false 0

typedef struct
{
  pthread_t threads[NUM_OF_THREADS];
  GtkTextBuffer *buffer;
  string txt;
} CONTEXT;

CONTEXT context;

void work(void);
void add_line(string str);

void *thread_func(void* param)
{
  int * pnum = (int*)param;
  int num = *pnum;

  {
    stringstream tmp;
    tmp << "thread " << num << " >>> ";
    add_line(tmp.str());
  }


  {
    stringstream tmp;
    tmp << "thread " << num << " <<< ";
    add_line(tmp.str());
  }
}

void create_thread(int num, int wait)
{
  pthread_attr_t attr;
  pthread_attr_init( &attr );
  pthread_t* p_tid;
  p_tid = context.threads + num;
  
  pthread_create( p_tid, &attr, thread_func, (void*)(&num) );

  if(wait == true)
    { //wait for thread ends
      pthread_join(*p_tid, NULL);
    }
}

static void
activate (GtkApplication* app,
	  gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *view;
  //GtkTextBuffer *buffer;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);

  //text widget

  view = gtk_text_view_new ();
  context.buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  add_line(string("Hello!"));

  /* Change left margin throughout the widget */
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (view), 30);


  gtk_container_add (GTK_CONTAINER (window), view);

  gtk_widget_show_all (window);

  work();
}

void add_line(string str)
{
  context.txt += str + string("\n");
  gtk_text_buffer_set_text (context.buffer, context.txt.c_str(), context.txt.length());
}

void work(void)
{
  create_thread(1, true);
  
  create_thread(2, true);
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
