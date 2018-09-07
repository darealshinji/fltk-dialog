/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// Tests:
// (echo 40; sleep 1; echo 80; sleep 1; echo 99; sleep 1; echo '#Done' && echo '100') | ./fltk-dialog --progress
// (echo '#Work in progress... ' && sleep 4 && echo '#Work in progress... done.' && echo 'STOP') | ./fltk-dialog --progress --pulsate

/*
(echo '#1/3'; echo 40; sleep 1; echo 80; sleep 1; echo 100; sleep 1; \
 echo '#2/3'; echo 40; sleep 1; echo 80; sleep 1; echo 100; sleep 1; \
 echo '#3/3'; echo 40; sleep 1; echo 80; sleep 1; echo '#Done.'; echo 100) | \
 ./fltk-dialog --progress --multi=3
*/

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

#include <fstream>
#include <iostream>
#include <string>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "fltk-dialog.hpp"

class loop_bar : public Fl_Widget
{
  /* values between 0.0 and 1.0 */
  double slider_size_, value_;

protected:
  void draw();

public:
  loop_bar(int x, int y, int w, int h);

  double slider_size() const { return slider_size_; }
  void slider_size(double v) { slider_size_ = (v > 0.0 && v < 1.0) ? v : 0.2; }

  double value() const { return value_; }
  void value(double v) { value_ = v; }
};

void loop_bar::draw()
{
  int dx, dy, dw, dh, sw, bx1, bx2, by, bh;
  double max;

  /* box borders */
  dx = Fl::box_dx(box());
  dy = Fl::box_dy(box());
  dw = Fl::box_dw(box());
  dh = Fl::box_dh(box());

  /* main box */
  fl_push_clip(x(), y(), w(), h());
  draw_box(box(), x(), y(), w(), h(), active_r() ? color() : fl_inactive(color()));
  fl_pop_clip();

  /* slider */
  if (value() > 0.0 && value() < 1.0) {
    sw = slider_size() * w();
    max = w() + sw;
    bx1 = x() + dx;
    bx2 = bx1 + (value() * max) - sw;
    by = y() + dy;
    bh = h() - dh;

    fl_push_clip(bx1, by, w() - dw, bh);
    draw_box(FL_FLAT_BOX, bx2, by, sw - dw, bh, active_r() ? selection_color() : fl_inactive(selection_color()));
    fl_pop_clip();
  }
}

loop_bar::loop_bar(int x, int y, int w, int h) : Fl_Widget(x, y, w, h)
{
  box(FL_DOWN_BOX);
  color(fl_darker(color()));
  selection_color(fl_lighter(FL_BLUE));
  value(0.0);
  slider_size(0.2);
}

static loop_bar         *lp = NULL;
static Fl_Double_Window *win = NULL;
static Fl_Box           *box = NULL;
static Fl_Return_Button *but_ok = NULL;
static Fl_Button        *but_cancel = NULL;
static Fl_Progress      *bar = NULL, *bar_main = NULL;
static int ret = 1;
static pthread_t t1, t2;

#ifdef WITH_FRIBIDI
static bool msg_alloc = false;
#endif

static
unsigned int percent = 0
,            multi = 1
,            multi_percent = 0
,            iteration = 0;

static bool running = true
,           pulsate = false
,           autoclose = false
,           hide_cancel = false;

static long pid = -1;

static void close_cb(Fl_Widget *, long p)
{
  pthread_cancel(t1);
  pthread_cancel(t2);
  win->hide();
  ret = p;
#ifdef WITH_FRIBIDI
  if (msg_alloc && msg) {
    delete msg;
  }
#endif
}

static void cancel_cb(Fl_Widget *o)
{
  if (pid > getpid()) {
    kill(pid, 1);
  }
  close_cb(o, 1);
}

static void progress_finished(void)
{
  if (autoclose) {
    close_cb(NULL, 0);
  } else {
    if (!hide_cancel) {
      but_cancel->deactivate();
    }
    but_ok->activate();
  }

  if (pulsate) {
    lp->value(0.0);
    lp->deactivate();
  }
}

static void parse_line(const char *ch)
{
  if (running) {
    if (ch[0] == '#' && ch[1] != '\0') {
      /* "#comment" line found, change the label */
#ifdef WITH_FRIBIDI
      char *tmp = NULL;
      if (use_fribidi) {
        tmp = fribidi_parse_line(ch + 1);
      }
      if (tmp) {
        box->copy_label(tmp);
        delete tmp;
      } else
#endif
      {
        box->copy_label(ch + 1);
      }
    } else if (!pulsate && ch[0] >= '0' && ch[0] <= '9') {
      char buf[16] = {0};

      /* number found, update the progress bar */
      percent = atoi(ch);
      if (percent >= 100) {
        percent = 100;
        running = multi > 1;
        iteration++;
      }
      snprintf(buf, sizeof(buf), "%d%%", percent);
      bar->value(percent);
      bar->copy_label(buf);

      /* update the main progress bar too if --multi=n was given */
      if (multi > 1) {
        if (percent == 100) {
          /* reset % for next iteration */
          percent = 0;
        }
        multi_percent = iteration * 100 + percent;
        if (multi_percent >= multi * 100) {
          multi_percent = multi * 100;
          running = false;
        }
        snprintf(buf, sizeof(buf), "%d%%", (multi_percent / multi));
        bar_main->value(multi_percent);
        bar_main->copy_label(buf);
      }
    } else if (pulsate && strcmp(ch, "STOP") == 0) {
      /* stop now */
      running = false;
    }
  }

  /* should be a separate "if"-statement, so we can finish
   * immediately after a "STOP" signal was sent */
  if (!running) {
    progress_finished();
  }

  Fl::redraw();
}

extern "C" void *progress_getline(void *)
{
  std::string line;
  while (std::getline(std::cin, line)) {
    Fl::lock();
    parse_line(line.c_str());
    Fl::unlock();
    Fl::awake(win);
  }
  return nullptr;
}

extern "C" void *pulsate_bar_thread(void *)
{
  while (running) {
    Fl::lock();

    double val = lp->value() + 0.001;
    if (val > 1.0) {
      val = 0.0;
    }
    lp->value(val);
    Fl::redraw();

    if (pid > getpid() && kill(pid, 0) == -1) {
      running = false;  /* the watched process has stopped */
      pid = -1;
    }

    Fl::unlock();
    Fl::awake(win);

    /* has an effect on the pulsate speed */
    usleep(1200);
  }

  if (!running) {
    progress_finished();
    Fl::redraw();
  }

  return nullptr;
}

int dialog_progress(bool pulsate_, unsigned int multi_, long pid_, bool autoclose_, bool hide_cancel_)
{
  Fl_Group *g;
  Fl_Box *dummy;
  int h = 140, offset = 0, range = 80;

#ifdef WITH_FRIBIDI
  if (msg && use_fribidi && (msg = fribidi_parse_line(msg)) != NULL) {
    msg_alloc = true;
  }
#endif

  if (!msg) {
    msg = "Progress indicator";
  }

  if (!title) {
    title = "FLTK progress window";
  }

  pulsate = pulsate_;
  multi = pulsate ? 1 : multi_;
  pid = pid_;
  autoclose = autoclose_;
  hide_cancel = hide_cancel_;

  if (hide_cancel && autoclose) {
    h -= 36;
  }

  if (multi > 1) {
    offset = 40;
  }

  win = new Fl_Double_Window(320, h + offset, title);
  win->callback(cancel_cb);
  {
    g = new Fl_Group(0, 0, 320, h + offset);
    {
      box = new Fl_Box(0, 10, 10, 30, msg);
      box->box(FL_NO_BOX);
      box->align(FL_ALIGN_RIGHT);

      if (pulsate) {
        lp = new loop_bar(10, 50, 300, 30);
      } else {
        if (multi > 1) {
          bar_main = new Fl_Progress(10, 50, 300, 30, "0%");
          bar_main->minimum(0);
          bar_main->maximum(multi * 100);
          bar_main->color(fl_darker(FL_GRAY));
          bar_main->selection_color(fl_lighter(FL_BLUE));
          bar_main->labelcolor(FL_WHITE);
          bar_main->value(0);
        }
        bar = new Fl_Progress(10, 50 + offset, 300, 30, "0%");
        bar->minimum(0);
        bar->maximum(100);
        bar->color(fl_darker(FL_GRAY));
        bar->selection_color(fl_lighter(FL_BLUE));
        bar->labelcolor(FL_WHITE);
        bar->value(0);
      }

      if (hide_cancel && autoclose) {
        dummy = new Fl_Box(10, 81 + offset, 300, 1);
      } else {
        int but_w = 0;
        int but_x = win->w() - 10;

        if (!hide_cancel) {
          range = but_w = measure_button_width(fl_cancel, 20);
          but_cancel = new Fl_Button(win->w() - 10 - but_w, 104 + offset, but_w, 26, fl_cancel);
          but_cancel->callback(cancel_cb);
          but_x = but_cancel->x() - 1;
        }

        if (!autoclose) {
          but_w = measure_button_width(fl_ok, 40);
          but_x = hide_cancel ? win->w() : but_cancel->x();
          but_ok = new Fl_Return_Button(but_x - 10 - but_w, 104 + offset, but_w, 26, fl_ok);
          but_ok->deactivate();
          but_ok->callback(close_cb, 0);
          but_x = but_ok->x() - 1;
          range = win->w() - but_x - 30;
        }

        dummy = new Fl_Box(but_x, 103 + offset, 1, 1);
      }
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(win, g);
  set_size_range(win, range, win->h());
  set_position(win);
  win->end();

  Fl::lock();

  set_taskbar(win);
  win->show();
  set_undecorated(win);
  set_always_on_top(win);

  if (pulsate) {
    /* the pulsating bar is running in its own thread */
    pthread_create(&t1, 0, &pulsate_bar_thread, nullptr);
  }
  pthread_create(&t2, 0, &progress_getline, nullptr);

  Fl::run();

  return ret;
}

