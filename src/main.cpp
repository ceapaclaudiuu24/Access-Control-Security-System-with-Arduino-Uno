#include "AccessCtrl.h"

AccessCtrl ac;

void setup() { ac.begin(); }

void loop() { ac.task(); }
