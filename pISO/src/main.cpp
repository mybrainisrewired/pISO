#include <cppgpio.hpp>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>

#include "bitmap.hpp"
#include "controller.hpp"
#include "display.hpp"
#include "font.hpp"
#include "lvmwrapper.hpp"
#include "piso.hpp"

// Setup script sets up:
//   sudo vgcreate VolGroup00 /dev/sdb1
//   sudo lvcreate -l 100%FREE -T VolGroup00/thinpool
//  So we can basically just do:
//   sudo lvcreate -V 100G -T VolGroup00/thinpool -n volume0

int main() {
  if (wiringPiSetupGpio() == -1) {
    piso_error("Error while setting up GPIO: ", strerror(errno));
  }

  auto &piso = pISO::instance();

  auto &controller = Controller::instance();
  int counter = 0;
  controller.on_rotate = [&](Controller::Rotation rot) {
    counter += 1;
    if (counter > 10) {
      piso.add_drive(100 * 1024 * 1024);
    }
    std::cout << "rotated: " << (int)rot << std::endl;
  };
  controller.start();

  // auto text = render_text("The swift brown fox jumps over the lazy dog!");
  // Display::instance().update(text);

  GPIO::PushButton button(22, GPIO::GPIO_PULL::UP);
  button.f_pushed = [&]() { piso.on_select(); };
  button.start();

  std::this_thread::sleep_for(std::chrono::hours(1));
}