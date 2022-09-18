// ----------------------------------- menu.c -------------------------------------
#include "menu.h"

#include "framebf.h"

// Draw the team banner
void team_banner() {
  drawString(MARGIN, MARGIN - 5, "[EEET2490] Embedded Systems", 0x07, 1);
  drawString(WIDTH - MARGIN - 5 - (23 * 8), MARGIN, "RMIT University Vietnam", 0x08, 1);

  drawRect(MARGIN, 545, WIDTH - MARGIN, 575, 0xb0, 1);                    // cyan fill black border
  drawRect(MARGIN + 5, 550, WIDTH - MARGIN - 5, 580, 0xf0, 1);            // white fill black border
  drawString(MARGIN + 25, 560, "Nguyen Minh Trang (s3751450)", 0xf9, 1);  // indigo text white bg
  drawString(MARGIN + 295, 560, "Tran Kim Long (s3755614)", 0xfc, 1);     // red text white bg
  drawString(MARGIN + 530, 560, "Truong Cong Anh (s3750788)", 0xf2, 1);   // green text white bg
}

// Display logo
void logo_init() {
  drawString((WIDTH / 2) - 110, 130, "CHICKEN", 0x0b, 5);
  drawString((WIDTH / 2) - 130, 200, "INVADERS", 0x0b, 5);
  drawString((WIDTH / 2) - 130, 120, "CHICKEN", 0x0f, 5);
  drawString((WIDTH / 2) - 150, 190, "INVADERS", 0x0f, 5);
  drawString((WIDTH / 2) - 150, 250, "(Low Budget Edition)", 0x0f, 2);
  drawString((WIDTH / 2) - 185, 300, "+-----+-----+-----+-----+", 0x0f, 2);
}

// Display menu
void menu_init() {
  drawString((WIDTH / 2) - 93, 350, "NEW GAME", 0x0b, 3);
  drawString((WIDTH / 2) - 127, 400, "HOW TO PLAY", 0x0f, 3);
  drawString((WIDTH / 2) - 185, 450, "(Press <W> or <S> to navigate through the options)", 0x0f, 1);
}

// Display <HOW TO PLAY> details
void howtoplay_details() {
  drawString((WIDTH / 2) - 120, 50, "HOW TO PLAY", 0x0b, 3);
  drawString((WIDTH / 2) - 340, 100, "+-----+-----+-----+-----+-----+-----+-----+", 0x0f, 2);
  drawString((WIDTH / 2) - 350, 150, "Evil chickens are invading space, soon they", 0x0f, 2);
  drawString((WIDTH / 2) - 380, 200, "will make it to our beloved Earth. Your duty is", 0x0f, 2);
  drawString((WIDTH / 2) - 380, 250, "to eliminate all of them before they can do so!", 0x0f, 2);
  drawString((WIDTH / 2) - 350, 300, "Use <W><A><S><D> buttons on your keyboard to", 0x0f, 2);
  drawString((WIDTH / 2) - 380, 350, "control the ship to shoot the chickens while", 0x0f, 2);
  drawString((WIDTH / 2) - 380, 400, "avoiding their eggs.", 0x0f, 2);
  drawString((WIDTH / 2) - 350, 450, "Are you ready?!! Let's hunt some chickens!!!", 0x0f, 2);
  drawString((WIDTH / 2) - 220, 525, "(Press <M> to return to menu)", 0x0b, 2);
}
