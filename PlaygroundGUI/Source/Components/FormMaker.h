/*
 * Automaton Playground
 * Copyright (C) 2019 Asen Kovachev (@asenski, GitHub: akovachev)
 *
 * Automaton Playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Automaton Playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Automaton Playground.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class FormMaker:
  public Component,
  public Button::Listener,
  public TextEditor::Listener {
 public:
  FormMaker();
  ~FormMaker();

  void paint(Graphics&) override;
  void resized() override;

 protected:
  OwnedArray<Component> components;

  void addComponent(Component* c) {
    components.add(c);
    addAndMakeVisible(c);
  }

  TextButton* TB(String text, int x, int y, int w, int h) {
    TextButton* tb = new TextButton(text);
    tb->addListener(this);
    addComponent(tb);
    tb->setBounds(x, y, w, h);

    tb->setColour(TextButton::textColourOffId,  Colours::black);
    tb->setColour(TextButton::textColourOnId,   Colours::black);
    tb->setColour(TextButton::buttonColourId,   Colours::white);
    tb->setColour(TextButton::buttonOnColourId, Colours::cyan.brighter());

    return tb;
  }

  int GTB(int gid, int def, StringArray texts, int x, int y, int w, int h) {
    int firstButtonIndex = components.size();
    for (unsigned int i = 0; i < texts.size(); i++) {
      String text = texts[i];
      TextButton* tb = TB(text, x, y, w, h);
      tb->setClickingTogglesState(true);
      tb->setRadioGroupId(gid);
      if (i == def) {
        tb->setToggleState(true, dontSendNotification);
      }

      tb->setConnectedEdges(
          ((i == 0) ? 0 : Button::ConnectedOnLeft) |
          ((i == (texts.size() - 1)) ? 0 : Button::ConnectedOnRight));

      x += w;
    }
  }

  Label* LBL(String text, int x, int y, int w, int h) {
    Label* lbl = new Label("", text);
    addComponent(lbl);
    lbl->setBounds(x, y, w, h);
    lbl->setColour(Label::textColourId, Colours::white);
    return lbl;
  }

  TextEditor* TXT(String name, int x, int y, int w, int h) {
    TextEditor* txt = new TextEditor(name);
    txt->addListener(this);
    addComponent(txt);
    txt->setBounds(x, y, w, h);
    // txt->setColour(TextEditor::backgroundColourId, Colours::black);
    // txt->setColour(TextEditor::textColourId, Colours::white);
    // txt->setJustificationType(Justification::centred);
    return txt;
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FormMaker)
};
