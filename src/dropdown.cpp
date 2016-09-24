#include <FL/Fl.H>
#include <FL/fl_ask.H>  /* fl_ok, fl_cancel */
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "misc/split.hpp"
#include "fltk-dialog.hpp"

Fl_Choice *dropdown_entries;
std::vector<std::string> dropdown_v;

static void dropdown_exit0_cb(Fl_Widget*)
{
  int item = dropdown_entries->value();

 /* a bit stupid, but I couldn't find a way to set this as a
  * callback parameter, so instead dropdown_return_number
  * is set to anything but NULL when the command line
  * option '--return-number' is given
  */
  if (dropdown_return_number != NULL)
  {
    std::cout << (item + 1) << std::endl;
  }
  else
  {
    std::cout << dropdown_v[item] << std::endl;
  }

  delete dropdown_entries;
  exit(0);
}

static void dropdown_exit1_cb(Fl_Widget*)
{
  delete dropdown_entries;
  exit(1);
}

int dialog_dropdown(std::string dropdown_list)
{
  int winw = 320;
  int droph = 30;
  int bord = 10;
  int textheight = 18;
  int butw = 100;
  int buth = 26;

  if (msg == NULL)
  {
    msg = (char *)"Select an option";
  }

  int boxh = textheight + bord*2;

  if (title == NULL)
  {
    title = (char *)"FLTK dropdown menu dialog";
  }

  split(dropdown_list, DEFAULT_DELIMITER, dropdown_v);

  size_t dropdown_size = dropdown_v.size();

  if (dropdown_size <= 1)
  {
    msg = (char *)"ERROR: need at least 2 entries";
    dialog_fl_message(ALERT);
    return 1;
  }

  Fl_Menu_Item dropdown_menu_items[dropdown_size];

  for (size_t i = 0; i <= dropdown_size; ++i)
  {
    if (i < dropdown_size)
    {
      if (dropdown_v[i] == "")
      {
        dropdown_menu_items[i].text = (char *)"<EMPTY>";
      }
      else
      {
        dropdown_menu_items[i].text = dropdown_v[i].c_str();
      }
    }
    else
    {
      /* { 0,0,0,0,0,0,0,0,0 } */
      dropdown_menu_items[i].text = 0;
    }

    dropdown_menu_items[i].shortcut_ = 0;
    dropdown_menu_items[i].callback_ = 0;
    dropdown_menu_items[i].user_data_ = 0;
    dropdown_menu_items[i].flags = 0;
    dropdown_menu_items[i].labeltype_ = (i == dropdown_size) ? 0 : FL_NORMAL_LABEL;
    dropdown_menu_items[i].labelfont_ = 0;
    dropdown_menu_items[i].labelsize_ = (i == dropdown_size) ? 0 : 14;
    dropdown_menu_items[i].labelcolor_ = 0;
  }

  Fl_Window *w = new Fl_Window(winw, boxh+droph+bord*3+textheight, title);
  w->begin();
  w->callback(dropdown_exit1_cb);
  {
    Fl_Button *ok = new Fl_Button(winw-butw*2-bord*2, boxh+textheight+buth,
                                  butw, buth, fl_ok);
    ok->callback(dropdown_exit0_cb);

    Fl_Button *cancel = new Fl_Button(winw-butw-bord, boxh+textheight+buth,
                                      butw, buth, fl_cancel);
    cancel->callback(dropdown_exit1_cb);

    dropdown_entries = new Fl_Choice(bord, boxh, winw-bord*2, droph, msg);
    dropdown_entries->down_box(FL_BORDER_BOX);
    dropdown_entries->align(FL_ALIGN_TOP_LEFT);
    dropdown_entries->menu(dropdown_menu_items);
  }
  w->end();
  w->show();

  return Fl::run();
}

