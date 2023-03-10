#pragma once

#include <array>
#include <memory>
#include <system_error>

namespace e00 {
class Bitmap : public Resource {

protected:

public:
  ~Bitmap() override = default;

  virtual bool HasPalette() const { return false; };

  virtual std::error_code SetPalette(const std::array<Color, 255>& palette) = 0;

  virtual Color GetPaletteColor(uint8_t index) const = 0;

  virtual Vec2D<uint16_t> Size() const = 0;

  /*virtual void set_clip() = 0;
  virtual void acquire() = 0;
  virtual void release() = 0;

  virtual std::shared_ptr<Bitmap> create_sub_bitmap(int x, int y, int width, int height) = 0;
  virtual std::shared_ptr<Bitmap> created_sub_bitmap() = 0;

  virtual int getpixel(int x, int y) = 0;
  virtual void putpixel(int x, int y, int color) = 0;

  virtual void vline(int x, int y_1, int y2, int color) = 0;
  virtual void hline(int x1, int y, int x2, int color) = 0;
  virtual void hfill(int x1, int y, int x2, int color) = 0;
  virtual void line(int x1, int y_1, int x2, int y2, int color) = 0;
  virtual void fastline(int x1, int y_1, int x2, int y2, int color) = 0;
  virtual void rectfill(int x1, int y_1, int x2, int y2, int color) = 0;
  virtual void triangle(int x1, int y_1, int x2, int y2, int x3, int y3, int color) = 0;

  virtual void draw_sprite(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_256_sprite(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_sprite_v_flip(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_sprite_h_flip(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_sprite_vh_flip(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_trans_sprite(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_trans_rgba_sprite(std::shared_ptr<Bitmap> sprite, int x, int y) = 0;
  virtual void draw_lit_sprite(std::shared_ptr<Bitmap> sprite, int x, int y, int color) = 0;
  virtual void draw_rle_sprite(const struct RLE_SPRITE *sprite, int x, int y) = 0;
  virtual void draw_trans_rle_sprite(const struct RLE_SPRITE *sprite, int x, int y) = 0;
  virtual void draw_trans_rgba_rle_sprite(const struct RLE_SPRITE *sprite, int x, int y) = 0;
  virtual void draw_lit_rle_sprite(const struct RLE_SPRITE *sprite, int x, int y, int color) = 0;
  virtual void draw_character(std::shared_ptr<Bitmap> sprite, int x, int y, int color, int bg) = 0;
  virtual void draw_glyph(const struct FONT_GLYPH *glyph, int x, int y, int color, int bg) = 0;

  virtual void blit_from_memory(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_to_memory(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_from_system(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_to_system(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_to_self(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_to_self_forward(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_to_self_backward(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void blit_between_formats(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;

  virtual void masked_blit(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
  virtual void clear_to_color(std::shared_ptr<Bitmap> bitmap, int color) = 0;
  virtual void pivot_scaled_sprite_flip(std::shared_ptr<Bitmap> sprite, fixed x, fixed y, fixed cx, fixed cy, fixed angle, fixed scale, int v_flip) = 0;
  virtual void do_stretch_blit(std::shared_ptr<Bitmap> source, std::shared_ptr<Bitmap> dest, int source_x, int source_y, int source_width, int source_height, int dest_x, int dest_y, int dest_width, int dest_height, int masked) = 0;
  virtual void draw_gouraud_sprite(std::shared_ptr<Bitmap> sprite, int x, int y, int c1, int c2, int c3, int c4) = 0;
  virtual void draw_sprite_end() = 0;
  virtual void blit_end() = 0;
  virtual void polygon(int vertices, const int *points, int color) = 0;
  virtual void rect(int x1, int y_1, int x2, int y2, int color) = 0;
  virtual void circle(int x, int y, int radius, int color) = 0;
  virtual void circlefill(int x, int y, int radius, int color) = 0;
  virtual void ellipse(int x, int y, int rx, int ry, int color) = 0;
  virtual void ellipsefill(int x, int y, int rx, int ry, int color) = 0;
  virtual void arc(int x, int y, fixed ang1, fixed ang2, int r, int color) = 0;
  virtual void spline(const int points[8], int color) = 0;
  virtual void floodfill(int x, int y, int color) = 0;

  virtual void polygon3d(int type, std::shared_ptr<Bitmap> texture, int vc, V3D *vtx[]) = 0;
  virtual void polygon3d_f(int type, std::shared_ptr<Bitmap> texture, int vc, V3D_f *vtx[]) = 0;
  virtual void triangle3d(int type, std::shared_ptr<Bitmap> texture, V3D *v1, V3D *v2, V3D *v3) = 0;
  virtual void triangle3d_f(int type, std::shared_ptr<Bitmap> texture, V3D_f *v1, V3D_f *v2, V3D_f *v3) = 0;
  virtual void quad3d(int type, std::shared_ptr<Bitmap> texture, V3D *v1, V3D *v2, V3D *v3, V3D *v4) = 0;
  virtual void quad3d_f(int type, std::shared_ptr<Bitmap> texture, V3D_f *v1, V3D_f *v2, V3D_f *v3, V3D_f *v4) = 0;

  virtual void draw_sprite_ex(std::shared_ptr<Bitmap> sprite, int x, int y, int mode, int flip) = 0;
  */
};
}// namespace e00
