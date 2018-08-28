/*
 * main.c
 *
 *  Created on: 11/16/16
 *  Created by: Kenny Zhou
 */

/* ECE 367 Lab Project: Kitchen Timer with Musical Alarm
 *
 * This program basically acts as a kitchen timer. The user will set the timer using the rotary angle sensor (limited to an hour)
 * which will be displayed on the 7 segment display
 * The timer will start counting down when the user presses button 1 on the launchpad.
 * During this time, the user may also press button 2 to add an extra minute to the remaining time if desired
 * When the timer finally reaches 0, the piezo buzzer will play a musical piece to notify the user that the time is up
 */

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "rotary.h"
#include "buzzer.h"

// Definition of the notes' frequecies in Hertz.
// The number corresponds to the pitch octave. Less = lower octave, more = higher octave
#define c_1 33
#define d1 37
#define e1 41
#define f1 44
#define g1 49
#define a1 55
#define b1 62
#define c_2 130
#define c 261 // c4
#define d 294
#define e 329
#define f 349
#define g 391
#define gS 415
#define a 440
#define aS 455
#define b 466
#define cH 523
#define cSH 554
#define dH 587
#define dSH 622
#define eH 659
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880
#define c7 2093
#define c8 4186
#define c6 1047
#define c5 523

// timer running state
static enum {Stopped, Paused, Started} timerState = Stopped;

// Status of the stopwatch
static struct {
	uint8_t s1;				// 1st digit of seconds
	uint8_t s2;				// 2nd digit of seconds
	uint8_t c1;				// 1st digit of centiseconds
	uint8_t c2;				// 2nd digit of centiseconds
} display = {0,0,0,0};

// Plays a note, takes in a frequency and an integer corresponding to the note duration as parameters
void
playNote(unsigned int note, unsigned int duration)
{
	// Need to balance the delay with the time (i.e cant increase delay too much without lowering the time some)

    int i;
    // This is the semiperiod of each note.
    // Use around 20000 as default, divide by 2 if you want to lower the note frequency sound by an octave
    // This is because the frequency's for each note octave are multiples of 2
    long delay = (long)(10800/note);

    // This is how much time we need to spend on the note
    // Divide delay to lengthen the note (make delay value smaller, which makes the time larger)
    // Multiply the delay by a factor to shorten the note
    long time = (long)((duration*150)/(delay));

	    for (i=0;i<time;i++)
		{
		buzzOn();
		waitUs(delay);
		buzzOff();
		waitUs(delay);
		}
	    waitMs(20); // slight delay to separate the single notes
}

// Plays all the playNotes at different frequencies and durations, and
// some delays to separate the various bits of the song
void
playMusic()
{
	// changing note durations here seems to require adjustments in the parameters for playNote
	// Divide the note frequencies by a multiple of 2 to get lower and lower pitches
	// Multiply to get higher pitches
	// Notes have to all be of the same octave!

	// Jingle Bells
	// verse 1 (dashing through the snow...)
	playNote(c/2, 500);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(f/2, 500);
	playNote(c/2, 900);

	playNote(c/2, 300);
	playNote(c/2, 300);
	playNote(c/2, 300);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(f/2, 500);
	playNote(d/2, 900);

	playNote(d/2, 500);
	playNote(aS/2, 500);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(e/2, 900);

	playNote(c/2, 500);
	playNote(c/2, 500);
	playNote(aS/2, 500);
	playNote(g/2, 500);
	playNote(a/2, 900);

	playNote(c/2, 500);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(f/2, 500);
	playNote(c/2, 900);

	playNote(c/2, 500);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(f/2, 500);
	playNote(d/2, 900);

	playNote(d/2, 500);
	playNote(d/2, 500);
	playNote(aS/2, 500);
	playNote(a/2, 500);
	playNote(g/2, 500);
	playNote(c/2, 500);
	playNote(c/2, 500);
	playNote(c/2, 500);
	playNote(c/2, 500);
	playNote(d/2, 500);
	playNote(c/2, 500);
	playNote(aS/2, 500);
	playNote(g/2, 500);
	playNote(f/2, 1200);

	// chorus (hey, jingle bells, jingle bells...)
	playNote(cSH/2, 750);
	playNote(a/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 550);
	playNote(a/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 550);

	playNote(a/2, 300);
	playNote(c/2, 400);
	playNote(f/2, 600);
	playNote(g/2, 200);
	playNote(a/2, 1000);

	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 250);
	playNote(a/2, 250);
	playNote(a/2, 400);
	playNote(g/2, 400);
	playNote(g/2, 400);
	playNote(a/2, 400);
	playNote(g/2, 500);
	playNote(cSH/2, 1000);

	// chorus pt 2
	playNote(a/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 550);
	playNote(a/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 550);

	playNote(a/2, 400);
	playNote(c/2, 350);
	playNote(f/2, 500);
	playNote(g/2, 300);
	playNote(a/2, 1000);

	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(aS/2, 400);
	playNote(a/2, 400);
	playNote(a/2, 400);

	playNote(a/2, 250);
	playNote(a/2, 250);
	playNote(cSH/2, 400);
	playNote(cSH/2, 400);
	playNote(a/2, 400);
	playNote(g/2, 400);
	playNote(f/2, 1000);

/*
	// twinkle twinkle
	playNote(c/2, 600);
	playNote(c/2, 600);
	playNote(g/2, 600);
	playNote(g/2, 600);
	playNote(a/2, 600);
	playNote(a/2, 600);
	playNote(g/2, 1500);

	playNote(f/2, 600);
	playNote(f/2, 600);
	playNote(e/2, 600);
	playNote(e/2, 600);
	playNote(d/2, 600);
	playNote(d/2, 600);
	playNote(c/2, 1500);
*/
}

// Continuously reads the output of the rotary angle sensor and shows it onto the 7 segment display
// Starts counting down second by second when timer has started
// Plays music to notify the user that the time has ended
void
timerUpdate(uint32_t time)
{
	/*
	// print ADC conversion number and its digits in raw and percentile form for debugging purposes
  	uprintf("%s %d\n\r", "ADC Digital Output: ", ADCinit());	// print analog to digital converted voltage signal from sensor
  	uprintf("%s %d\n\r", "ADC Digital Output: ", ADCinit()/68);
  	uprintf("%s %d\n\r", "ADC Digital Output digit 1: ", (ADCinit()/68)%10);			// first minute digit
  	uprintf("%s %d\n\r", "ADC Digital Output digit 1: ", ((ADCinit()/68)/10)%10);		// second minute digit*/

	// set the time of the timer using the rotary angle sensor
  	if(timerState == Stopped)
  	{
  	// set display struct variables as each individual digit of the digital number
	display.s1 = 0;
	display.s2 = 0;
	display.c1 = (ADCinit()/68)%10; // divide 4095 by 68 to get equal intervals for 60 minutes
	display.c2 = ((ADCinit()/68)/10)%10;

	// Continuously update the display of the angle on the rotary sensor
	seg7DigitUpdate(display.s1, display.s2, display.c1, display.c2);
  	}

	// start count down of the timer with wraparound
	if(timerState == Started)
	{
		uprintf("%s %d\n\r", "First digit: ", display.s1);
		uprintf("%s %d\n\r", "Second digit: ", display.s2);
		uprintf("%s %d\n\r", "Third digit: ", display.c1);
		uprintf("%s %d\n\r", "Fourth digit: ", display.c2);

		// decrement timer, wraparound when the digits = 0
		if(display.s1 == 0)
			{
			display.s2 = display.s2 - 1;
			display.s1 = 10;
			}
			if(display.s2 > 254) // 0 - 1 = 255, since time starts at xx:00, statement above makes display.s2 = 255, so wraparound
				{
				display.c1 = display.c1 - 1;
				display.s2 = 5;
				}
				if(display.c1 > 254)
					{
					display.c2 = display.c2 - 1;
					display.c1 = 9;
					}
							else
							{
							display.s1 = display.s1 - 1;
							}

			// Update the display after decrement
			seg7DigitUpdate(display.s1, display.s2, display.c1, display.c2);

			// play musical tune when timer reaches 0
			if(display.s1 == 0 && display.s2 == 0 && display.c1 == 0 && display.c2 == 0)
			{
			playMusic();
			timerState = Stopped;  // reset the timer, begins to read the rotary sensor again once finished with music
			}
	}

	// Timer is paused, just continuously display the current values in display struct
	else if (timerState == Paused)
		seg7DigitUpdate(display.s1, display.s2, display.c1, display.c2);

	// Call back after 250 milliseconds
	schdCallback(timerUpdate, time + 150);
}

// Event driven code for checking push button
// Button 1 starts the countdown of the timer after the user has set the desired time, pressing it again will pause/start the timer
// Button 2 adds an additional minute to the current time on the timer
void
checkPushButton(uint32_t time)
{
	int code = pbRead();
	uint32_t delay;

	  switch(code)
	  {
		  // button1 is pressed
		  case 1:
		  uprintf("%s\n\r", "Start countdown!");

		  if(timerState == Started)
		  {
			  uprintf("%s\n\r", "Paused");
			  timerState = Paused;
		  }
		  else
		  {
			  uprintf("%s\n\r", "Resumed");
			  timerState = Started;
		  }
		  delay = 250;
		  break;

	  // button2 is pressed
	  case 2:
		  uprintf("%s\n\r", "Off");

		  if(display.c1 == 9)
		  {
			  display.c1 = 0;
			  display.c2 = display.c2 + 1;
		  }

		  else
			  display.c1 = display.c1 + 1;

		  delay = 250;
		  break;

	 default:
		delay = 10;
	  }
	schdCallback(checkPushButton, time + delay);
}

int main(void)
{
	// initialize drivers for i/o peripherals
	lpInit();
	seg7Init();
	rotaryInit();
	buzzInit();
	//playMusic();

	uprintf("%s\n\r", "Lab Project: Kitchen Timer with Musical Alarm.");

	schdCallback(timerUpdate, 1000);
	schdCallback(checkPushButton, 1005);

	while (true) {
		schdExecute();
	}
}
