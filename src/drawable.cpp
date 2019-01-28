#include "drawable.hpp"

drawable::drawable(int init_width, int init_height, Uint32 fill) {
  apperance = std::shared_ptr<SDL_Surface>(
    SDL_CreateRGBSurface(
      0,
      (init_width > 0)? init_width : 1,
      (init_height > 0)? init_height : 1,
      32,
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x000000ff
    ),
    []( SDL_Surface * ptr ) {
      SDL_FreeSurface( ptr );
    }
  );

    unsigned char * pixels_ = (unsigned char*)(apperance->pixels);
		int width = apperance->w;
		int height = apperance->h;
		int pSize = apperance->format->BytesPerPixel;

		for(int j = 0; j<height; ++j)
			for(int i = 0; i<width; ++i)
			{
				*( pixels_ +width*pSize*j + i*pSize) = (fill & 0xff000000)>>24;
				*( pixels_ +width*pSize*j + i*pSize +1) = (fill & 0x00ff0000)>>16;
				*( pixels_ +width*pSize*j + i*pSize +2) = (fill & 0x0000ff00)>>8;
				*( pixels_ +width*pSize*j + i*pSize +3) = fill & 0x000000ff;
      }
}

drawable::drawable(const std::string& file) {
  apperance = std::shared_ptr<SDL_Surface>(
    SDL_LoadBMP(file.c_str()),
    []( SDL_Surface * ptr ) {
      SDL_FreeSurface( ptr );
    }
  );
}

drawable& drawable::draw(std::vector<std::tuple<point<int>,Uint32>> v) {

        int width = apperance->w;
        int height = apperance->h;
        int pSize = apperance->format->BytesPerPixel;
        unsigned char * pixels_ = (unsigned char*)(apperance->pixels);

        std::for_each(v.begin(),v.end(), [=](auto element){

          if( width>= std::get<0>(element).x() || std::get<0>(element).x()>=0 )
            if( height>=std::get<0>(element).y() || std::get<0>(element).y()>=0 ) {


              int location = width*pSize*std::get<0>(element).y() + std::get<0>(element).x()*pSize;
              Uint32 RGBA = std::get<1>(element);


              *( pixels_ + location)
              = (RGBA & 0xff000000)>>24;
              *( pixels_ + location +1)
              = (RGBA & 0x00ff0000)>>16;
              *( pixels_ + location +2)
              = (RGBA & 0x0000ff00)>>8;
              *( pixels_ + location +3)
              = RGBA & 0x000000ff;
            }
        });
        return *this;
}

drawable& drawable::resize(int new_left, int new_right, int new_top, int new_bottom, Uint32 fill) {

  if(
    (new_left == 0) &&
    (new_right == 0) &&
    (new_top == 0) &&
    (new_bottom == 0)
  ) {
    return *this;
  }

  std::shared_ptr<SDL_Surface> resized(
    SDL_CreateRGBSurface(
      0,
      (new_left+new_right > 0)? new_left+new_right : 1,
      (new_top+new_bottom > 0)? new_top+new_bottom : 1,
      32,
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x000000ff
    ),
    []( SDL_Surface * ptr ) {
      SDL_FreeSurface( ptr );
    }
  );

  {
    unsigned char * pixels_ = (unsigned char*)(resized->pixels);
    int width = resized->w;
    int height = resized->h;
    int pSize = resized->format->BytesPerPixel;

    for(int j = 0; j<height; ++j)
      for(int i = 0; i<width; ++i)
      {
        *( pixels_ +width*pSize*j + i*pSize) = (fill & 0xff000000)>>24;
        *( pixels_ +width*pSize*j + i*pSize +1) = (fill & 0x00ff0000)>>16;
        *( pixels_ +width*pSize*j + i*pSize +2) = (fill & 0x0000ff00)>>8;
        *( pixels_ +width*pSize*j + i*pSize +3) = fill & 0x000000ff;
      }
  }

  SDL_Rect srcrect;
  SDL_Rect dstrect;

  srcrect.x = 0;
  srcrect.y = 0;
  srcrect.w = apperance->w;
  srcrect.h = apperance->h;

  dstrect.x = new_left;
  dstrect.y = new_top;

  SDL_BlitSurface(apperance.get(), &srcrect, resized.get(), &dstrect);

  apperance = resized;

  return *this;
}

void drawable::paste_to_renderer(
  std::shared_ptr<SDL_Renderer> renderer,
  const point<int> at,
  const double angle
) {
    // this.resize(
    //   (at.x() > 0) ? 0 : -at.x(),
    //   (src.width() + at.x() > this.width()) ? src.width() + at.x() - this.width() : 0,
    //   (at.y() > 0) ? 0 : -at.y(),
    //   (src.height() + at.y() > this.height()) ? src.height() + at.y() - this.height() : 0,
    // );

    SDL_Rect dstrect;

    dstrect.x = at.x();
    dstrect.y = at.y();
    dstrect.w = apperance->w;
    dstrect.h = apperance->h;

    if(!texture)
      createTexture(renderer);

    SDL_RenderCopyEx(renderer.get(),
                     texture.get(),
                     NULL,
                     &dstrect,
                     angle,
                     NULL,
                     SDL_FLIP_NONE);
}

drawable drawable::clone() {

  unsigned char * pixels_ = (unsigned char*)(apperance->pixels);
  int width = apperance->w;
  int height = apperance->h;
  int pSize = apperance->format->BytesPerPixel;

  drawable cloned(width,height);
  unsigned char * pixels_cloned = (unsigned char*)(cloned.apperance->pixels);

  for(int j = 0; j<height; ++j)
    for(int i = 0; i<width; ++i)
    {
      *( pixels_cloned +width*pSize*j + i*pSize) = *( pixels_ +width*pSize*j + i*pSize);
    }

  return cloned;
}

drawable& drawable::createTexture(std::shared_ptr<SDL_Renderer> renderer) {
  texture = std::shared_ptr<SDL_Texture>(
    SDL_CreateTextureFromSurface(renderer.get(), apperance.get()),
    []( SDL_Texture* ptr ) {
      SDL_DestroyTexture(ptr);
    }
  );

  return *this;
}

int drawable::height() const {
  return apperance->h;
}

int drawable::width() const {
  return apperance->w;
}
