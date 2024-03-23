//-*- C++ -*-
//
// Copyright (C) 2024 Scrap Computing
//

#include "Button.h"

/// \Returns true if we "release both", which currently only if both release
/// or if one is released and the other is pressed.
bool bothRelease(ButtonState BS1, ButtonState BS2) {
  if ((BS1 == ButtonState::Release || BS1 == ButtonState::MedRelease) &&
      (BS2 == ButtonState::Release || BS2 == ButtonState::MedRelease))
    return true;
  return ((BS1 == ButtonState::Release || BS1 == ButtonState::MedRelease) &&
          BS2 == ButtonState::Pressed) ||
         ((BS2 == ButtonState::Release || BS2 == ButtonState::MedRelease) &&
          BS1 == ButtonState::Pressed);
}

bool bothLongPress(ButtonState BS1, ButtonState BS2) {
  return BS1 == ButtonState::LongPress && BS2 == ButtonState::Pressed;
}
