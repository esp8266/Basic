#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>
#include <fonts/SystemFont5x7.h>

/*
 * This sketch just prints a chunk of the famous "filler text" Lorem
 * Ipsum[1] to the OLED display, scrolling as it goes.
 *
 * The sketch pauses after each word for effect.
 *
 * Using the font "SystemFont5x7" and the full screen, the display can
 * show 25 characters across and 16 rows down.
 *
 * [1] https://en.wikipedia.org/wiki/Lorem_ipsum
 *     http://www.lipsum.com
 */

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

OLED oled(pin_cs, pin_dc, pin_reset);
OLED_TextBox box(oled);

// The IPSUM text we're using is quite big so we store it in program memory
// See http://arduino.cc/en/Reference/PROGMEM for an explanation.
static const char IPSUM[] PROGMEM  =
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
  "Donec a diam lectus. Sed sit amet ipsum mauris. "
  "Maecenas congue ligula ac quam viverra nec consectetur ante "
  "hendrerit. Donec et mollis dolor. Praesent et diam eget libero "
  "egestas mattis sit amet vitae augue."
  "Nam tincidunt congue enim, ut porta lorem lacinia consectetur. "
  "Donec ut libero sed arcu vehicula ultricies a non tortor. Lorem "
  "ipsum dolor sit amet, consectetur adipiscing elit. Aenean ut "
  "gravida lorem. Ut turpis felis, pulvinar a semper sed, adipiscing "
  "id dolor. Pellentesque auctor nisi id magna consequat sagittis. "
  "Curabitur dapibus enim sit amet elit pharetra tincidunt feugiat "
  "nisl imperdiet. Ut convallis libero in urna ultrices accumsan. "
  "Donec sed odio eros. Donec viverra mi quis quam pulvinar at malesuada "
  "arcu rhoncus. Cum sociis natoque penatibus et magnis dis parturient "
  "montes, nascetur ridiculus mus. In rutrum accumsan ultricies. "
  "Mauris vitae nisi at sem facilisis semper ac in est.\n\n"

  "Vivamus "
  "fermentum semper porta. Nunc diam velit, adipiscing ut tristique "
  "vitae, sagittis vel odio. Maecenas convallis ullamcorper ultricies. "
  "Curabitur ornare, ligula semper consectetur sagittis, nisi diam "
  "iaculis velit, id fringilla sem nunc vel mi. Nam dictum, odio "
  "nec pretium volutpat, arcu ante placerat erat, non tristique "
  "elit urna et turpis. Quisque mi metus, ornare sit amet fermentum "
  "et, tincidunt et orci. Fusce eget orci a orci congue vestibulum. "
  "Ut dolor diam, elementum et vestibulum eu, porttitor vel elit. "
  "Curabitur venenatis pulvinar tellus gravida ornare. Sed et erat "
  "faucibus nunc euismod ultricies ut id justo. Nullam cursus suscipit "
  "nisi, et ultrices justo sodales nec. Fusce venenatis facilisis "
  "lectus ac semper. Aliquam at massa ipsum. Quisque bibendum purus "
  "convallis nulla ultrices ultricies. Nullam aliquam, mi eu aliquam "
  "tincidunt, purus velit laoreet tortor, viverra pretium nisi quam "
  "vitae mi. Fusce vel volutpat elit. Nam sagittis nisi dui.\n\n"

  "Suspendisse "
  "lectus leo, consectetur in tempor sit amet, placerat quis neque. "
  "Etiam luctus porttitor lorem, sed suscipit est rutrum non. Curabitur "
  "lobortis nisl a enim congue semper. Aenean commodo ultrices imperdiet. "
  "Vestibulum ut justo vel sapien venenatis tincidunt. Phasellus "
  "eget dolor sit amet ipsum dapibus condimentum vitae quis lectus. "
  "Aliquam ut massa in turpis dapibus convallis. Praesent elit lacus, "
  "vestibulum at malesuada et, ornare et est. Ut augue nunc, sodales "
  "ut euismod non, adipiscing vitae orci. Mauris ut placerat justo. "
  "Mauris in ultricies enim. Quisque nec est eleifend nulla ultrices "
  "egestas quis ut quam. Donec sollicitudin lectus a mauris pulvinar "
  "id aliquam urna cursus. Cras quis ligula sem, vel elementum mi. "
  "Phasellus non ullamcorper urna.\n\n"

  "Class aptent taciti sociosqu "
  "ad litora torquent per conubia nostra, per inceptos himenaeos. "
  "In euismod ultrices facilisis. Vestibulum porta sapien adipiscing "
  "augue congue id pretium lectus molestie. Proin quis dictum nisl. "
  "Morbi id quam sapien, sed vestibulum sem. Duis elementum rutrum "
  "mauris sed convallis. Proin vestibulum magna mi. Aenean tristique "
  "hendrerit magna, ac facilisis nulla hendrerit ut. Sed non tortor "
  "sodales quam auctor elementum. Donec hendrerit nunc eget elit "
  "pharetra pulvinar. Suspendisse id tempus tortor. Aenean luctus, "
  "elit commodo laoreet commodo, justo nisi consequat massa, sed "
  "vulputate quam urna quis eros. Donec vel.";

void setup() {
  oled.begin();
  oled.selectFont(SystemFont5x7);
  box.setForegroundColour(OLDLACE);
}

void loop() {
  box.clear();
  const char *lorem = IPSUM;
  while(char c = pgm_read_byte(lorem)) {
    box.print(c);
    if(c == ' ' or c == '\n') // Pause after each word or line
      delay(100);
    lorem++;
  }
  delay(2000);
}
