#ifndef _3D_H
#define _3D_H

#include "3d/Matrix.h"
#include "3d/CompositeGraphics.h"
#include "3d/Mesh.h"
#include "3d/Image.h"

#include "3d/skull.h"
#include "3d/venus.h"
#include "3d/logo3d.h"
#include "3d/dragon.h"
#include "3d/lablogo.h"

#include "3d/font6x8.h"

const int XRES = 160;
const int YRES = 80;

//Graphics using the defined resolution for the backbuffer and fixed buffer for triangles
CompositeGraphics graphics(XRES, YRES, 1337);

//Composite output using the desired mode (PAL/NTSC) and twice the resolution.
//It will center the displayed image automatically
// CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

Mesh<CompositeGraphics> model_venus(venus::vertexCount, venus::vertices, 0, 0, venus::triangleCount, venus::triangles, venus::triangleNormals);

Mesh<CompositeGraphics> model_dragon(dragon::vertexCount, dragon::vertices, 0, 0, dragon::triangleCount, dragon::triangles, dragon::triangleNormals);

Mesh<CompositeGraphics> model_skull(skull::vertexCount, skull::vertices, 0, 0, skull::triangleCount, skull::triangles, skull::triangleNormals);

Image<CompositeGraphics> image(lablogo::xres, lablogo::yres, lablogo::pixels);

Font<CompositeGraphics> font(6, 8, font6x8::pixels);

int color = ST77XX_WHITE;

void init_3d() {
  canvas.fillScreen(color); // fill screen bg
  sendGRAM();
  //initializing graphics double buffer
  graphics.init();
  //select font
  graphics.setFont(font);
}

void drawSkull()
{
  float compositePixelAspect = 1;
  image.draw(graphics, 30, 70);
  static Matrix perspective = Matrix::translation(graphics.xres / 2, graphics.yres / 2, 0) * Matrix::scaling(100 * compositePixelAspect, 100, 100) * Matrix::perspective(90, 1, 10);
  static float u = 0;
  static float v = 0;
  u += 0.05;
  v += 0.01;
  Matrix rotation = Matrix::rotation(u, 0, 1, 0) * Matrix::rotation(v, 1, 0, 0);
  Matrix m0 = perspective * Matrix::translation(-4, 1.7 * 0, 7) * rotation * Matrix::scaling(6);
  model_skull.transform(m0, rotation);
  model_skull.drawTriangles(graphics, 40);
  graphics.flush();
}

void drawVenus() {
  float compositePixelAspect = 1;
  static Matrix perspective = Matrix::translation(graphics.xres / 2, graphics.yres / 2, 0) * Matrix::scaling(100 * compositePixelAspect, 100, 100) * Matrix::perspective(90, 1, 10);
  static float u = 0;
  u += 0.03;
  Matrix rotation = Matrix::rotation(-1.7, 1, 0, 0) * Matrix::rotation(u, 0, 0, 1);
  Matrix m0 = perspective * Matrix::translation(0, 1.7 * 0, 6) * rotation * Matrix::scaling(7);
  model_venus.transform(m0, rotation);
  model_venus.drawTriangles(graphics, 40);
  graphics.flush();
}

void drawDragon() {
  float compositePixelAspect = 1;
  static Matrix perspective = Matrix::translation(graphics.xres / 2, graphics.yres / 2, 0) * Matrix::scaling(100 * compositePixelAspect, 100, 100) * Matrix::perspective(90, 1, 10);
  static float u = 0;
  u += 0.03;
  Matrix rotation = Matrix::rotation(-1.8 + sin(u * 0.3) * 0.2, 1, 0, 0) * Matrix::rotation(u, 0, 0, 1);
  Matrix m0 = perspective * Matrix::translation(0, 1.7 * 0, 6) * rotation * Matrix::scaling(8);
  model_dragon.transform(m0, rotation);
  model_dragon.drawTriangles(graphics, 40);
  graphics.flush();
}

void draw() {
  static int lastMillis = 0;
  int t = millis();
  int fps = 1000 / (t - lastMillis);
  lastMillis = t;
  graphics.begin(54);
  int alter = (millis() / 5000) % 3;
  int tps;
  if (alter == 0) {
    drawVenus();
    tps = fps * model_venus.triangleCount;
  } else if (alter == 1) {
    drawDragon();
    tps = fps * model_dragon.triangleCount;
  } else if (alter == 2) {
    drawSkull();
    tps = fps * model_skull.triangleCount;
  }
  graphics.setTextColor(0xE75C); // nearly white
  graphics.setCursor(5, 2);
  graphics.print("free memory: ");
  // Serial.print("free memory: ");
  graphics.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  // Serial.println((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  graphics.print(" fps: ");
  // Serial.print(" fps: ");
  graphics.print(fps, 10, 2);
  // Serial.println(fps);
  graphics.print(" triangles/s: ");
  graphics.print(tps);
  graphics.end();
  sendGRAM();
  delay(1);
}

void page_3d() {
  draw();
}

#endif // _3D_H
