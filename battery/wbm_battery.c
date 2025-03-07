/* battery.c: program to be used in a custom waybar module.
 *
 * Please note that this file was updated in 2025 to target
 * the Framework 13 laptop with an AMD CPU. If you are having
 * issues using this program, please check the git history
 * and try to use the prior version. You may also need to change
 * the path for your battery in the constant variables below.
 *
 * Homepage: https://github.com/krathalan/waybar-modules
 *
 * Copyright (C) 2019-2025 Hunter Peavey
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

// Needed to convert strings to lowercase
#include <ctype.h>

// Needed to exit when a directory cannot be found
#include <stdlib.h>

// Constants
#define SCALE_FACTOR 1000000000000
#define STR_BUFFER_LEN 15
#define LARGE_STR_BUFFER_LEN 20

// Battery paths
#define PATH_CURRENT_NOW "/sys/class/power_supply/BAT1/current_now"
#define PATH_VOLTAGE_NOW "/sys/class/power_supply/BAT1/voltage_now"
#define PATH_CHARGE_FULL "/sys/class/power_supply/BAT1/charge_full"
#define PATH_CHARGE_NOW "/sys/class/power_supply/BAT1/charge_now"
#define PATH_STATE "/sys/class/power_supply/BAT1/status"

// Functions
float read_float_from_file(char fileToOpen[]);
void read_string_from_file(char fileToOpen[], char contentOfFile[]);

int main(void) {
  // Variables to fill
  float currentDischarge;
  float batteryPercentage;
  char wattDisplayString[STR_BUFFER_LEN];
  char batteryState[STR_BUFFER_LEN];

  // 1. Current discharge
  currentDischarge =
      read_float_from_file(PATH_CURRENT_NOW) * read_float_from_file(PATH_VOLTAGE_NOW) 
      / SCALE_FACTOR;

  // 2. Battery percentage
  batteryPercentage = read_float_from_file(PATH_CHARGE_NOW) /
                      read_float_from_file(PATH_CHARGE_FULL) * 100;

  // 3. Watt display string
  snprintf(wattDisplayString, sizeof(wattDisplayString), " (%0.0fW)",
           currentDischarge);

  // 4. Battery state
  read_string_from_file(PATH_STATE, batteryState);

  // Convert batteryState to lowercase as waybar CSS expects a lowercase value
  for (int i = 0; batteryState[i]; i++) {
    batteryState[i] = tolower(batteryState[i]);
  }

  // 5. Battery state icon
  // If the battery is charging or the wattage is less than 1W (i.e., 0), convert
  // wattage output to charging icon.
  if (strcmp(batteryState, "charging") == 0 || currentDischarge < 1) {
    strcpy(wattDisplayString, "󱐥");

    // Assert batteryState to charging if we got here through currentDischarge=0
    snprintf(batteryState, sizeof(batteryState), "charging");
  } else {
    // If battery is NOT charging,
    // and if battery is less than 30% charged,
    // change battery state to "critical"
    if (batteryPercentage < 30) {
      snprintf(batteryState, sizeof(batteryState), "critical");
    }
  }

  // 6. Final output
  printf("{\"text\": \"%s\", \"class\": \"%s\", \"percentage\": %0.0f}",
    wattDisplayString, batteryState, batteryPercentage);

  return 0;
}

float read_float_from_file(char fileToOpen[]) {
  float toReturn;
  FILE *fileToRead;

  fileToRead = fopen(fileToOpen, "r");

  if (fileToRead == NULL) {
    printf("Error reading file %s", fileToOpen);
    exit(0);
  }

  fscanf(fileToRead, "%f", &toReturn);

  fclose(fileToRead);

  return toReturn;
}

void read_string_from_file(char fileToOpen[], char contentOfFile[]) {
  FILE *fileToRead;

  fileToRead = fopen(fileToOpen, "r");

  if (fileToRead == NULL) {
    printf("Error reading file %s", fileToOpen);
    exit(0);
  }

  fscanf(fileToRead, "%s", contentOfFile);

  fclose(fileToRead);
}
