
#include "Screen.h"
#include <iostream>
#include <thread>
#include <chrono>

g3::Screen::Screen():
frontBuffer {Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, 640, 480)},
backBuffer {Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, 640, 480)},
targetFrameTime {33300000}
{
	// start frame time
	startFrameTime = clock_time();

	clear();

	// register idle function
	Glib::signal_idle().connect(sigc::mem_fun(*this, &Screen::on_idle));
}


void g3::Screen::clear()
{
	// Fill the buffer with color white
	frontBuffer->fill(0xffffffff);
	
	// Fill the buffer with color white
	backBuffer->fill(0xffffffff);
}

bool g3::Screen::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
	
	clear();

	Gtk::Allocation allocation = get_allocation();
	const int width  = allocation.get_width();
	const int height = allocation.get_height();

	// Draw the buffer
	Gdk::Cairo::set_source_pixbuf(cr, frontBuffer);
	cr->paint();

	return true;
}

/**
 * Returns a time point in nanoseconds.
 *
 * @return Time point in nanoseconds.
 */
unsigned long g3::Screen::clock_time()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	return (ts.tv_sec * 1000000000) + ts.tv_nsec;
}

/**
 * This idle callback function is executed as often as possible, 
 * hence it is ideal for processing intensive tasks.
 */
bool g3::Screen::on_idle()
{
	// finish current frame
	finishFrameTime = clock_time();

	// The elapsed frame time in nanoseconds.
	unsigned long elapsed = finishFrameTime - startFrameTime;

	//std::cout << "Elapsed: " << "Finish: " << finishFrameTime << " | start: " << startFrameTime << " | elapsed: " << elapsed << std::endl;

	if (elapsed <= targetFrameTime)
	{
		// sleep
		std::chrono::duration<float, std::nano> wait { targetFrameTime - elapsed };
		//std::cout << "Sleep for: " << wait.count() << std::endl;
		std::this_thread::sleep_for( wait );

		// redraw only if we were too fast in the prevoius frame.
		queue_draw();
	}
	else
	{
		// Here we were too slow so we will skip the next redrawing.
		std::cout << "Frame dropping: " << elapsed << std::endl;

	}
	
	// start new frame
	startFrameTime = clock_time();

	return true;
}


