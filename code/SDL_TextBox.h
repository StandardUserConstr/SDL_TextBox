#ifndef SECURITY_SDL_TEXTBOX0_H
#define SECURITY_SDL_TEXTBOX0_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>


#ifdef TEXBOX_DEBUG
#include <stdio.h>
#endif

class TextBoxClass
{
    //if u define variable "TEXBOX_DEBUG" before compilation/at compilation time
    //then u get access to debuge mode of function "TexBox";
    //if u using gnu compilator then for example u can add option "-D TEXBOX_DEBUG" at compilation time
    //or just define it at the beginning of using pre-processor instruction like "#define TEXTBOX_DEBUG"

    //CONST
        uint32_t base_beam_x = 0;
    //=================================

        uint32_t max_chars;

        uint32_t beam_timer0 = 0;
        uint32_t beam_timer1 = 0;
        uint8_t beam_animation_direction = 0;

        uint32_t double_click_timer0 = 0;
        uint32_t double_click_last_beam_position = 0;
        uint8_t double_click_enter = 0;
        bool do_double_click_ended = 1;

        uint32_t select_start_position_x = 0;
        uint32_t select_end_position_x = 0;
        int32_t select_base_mouse_x = 0;
        uint32_t how_many_chars_in_buffor;
        uint32_t at_what_position_is_beam;
        uint32_t width_of_text_texture = 0;

        uint8_t first_enter = 1;
        uint8_t focus = 0;
        uint8_t mouse_state = 0;
        uint8_t cursor_display = 0;

        SDL_Surface* main_surface = NULL;
        SDL_Texture* main_texture = NULL;
        SDL_Rect main_texture_rect;

        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t padding_left;
        uint16_t padding_up;
        SDL_Texture* beam = NULL;
        SDL_Rect rect_beam;
        SDL_Renderer* render;
        TTF_Font* main_font;
        uint32_t beam_delay_in_ms;
        uint8_t mode;
        uint8_t extra_char_allowed_to_mode;
        bool is_space_allowed;
        uint32_t double_click_max_delay_ms;
        SDL_Color solid_text_color;
        SDL_Color selected_text_color;
        SDL_Color selected_text_background_color;
        uint8_t solid_font_quality;
        uint8_t selected_font_quality;

        uint8_t do_data_should_be_safed_variable = 0;
        bool updated_textbox = 0;

        //tool
        //copy entire surface "src" to surface "dst" at coordinates "x" and "y";
        void combine_surfaces(uint32_t x,uint32_t y,SDL_Surface* src,SDL_Surface* dst);

    public:
        static const uint8_t MODE_LETTERS = 0;
        static const uint8_t MODE_NUMBERS = 1;
        static const uint8_t MODE_ALL = 2;
        static const uint8_t MODE_LETTERS_WITH_SYMBOLS = 3;
        static const uint8_t MODE_NUMBERS_WITH_SYMBOLS = 4;
        static const uint8_t MODE_SMALL_LETTERS = 5;
        static const uint8_t MODE_SMALL_LETTERS_WITH_NUMBERS = 6;

        static const uint8_t SOLID_FONT_QUALITY_STANDARD = 0;
        static const uint8_t SOLID_FONT_QUALITY_BLENDED = 1;

        static const uint8_t SELECTED_FONT_QUALITY_LCD = 0;
        static const uint8_t SELECTED_FONT_QUALITY_SHADED = 1;


        struct TextBoxStructure_settings
        {
            uint16_t x;
            uint16_t y;
            uint16_t w;
            uint16_t h;
            uint16_t padding_left = 0;
            uint16_t padding_up = 0;

            SDL_Texture* beam;
            SDL_Rect rect_beam;  //changing rect_beam.x dynamically causing bugs and should not be changed;
            SDL_Renderer* render;
            TTF_Font* main_font;

            uint32_t beam_delay_in_ms = 500;
            uint8_t mode = TextBoxClass::MODE_LETTERS; //MODE_LETTERS || MODE_NUMBERS || MODE_ALL || MODE_LETTERS_WITH_SYMBOLS ||MODE_NUMBERS_WITH_SYMBOLS
            uint8_t extra_char_allowed_to_mode = 0;
            bool is_space_allowed = 1;
            uint32_t double_click_max_delay_ms = 500;

            uint8_t solid_font_quality = SOLID_FONT_QUALITY_STANDARD;
            uint8_t selected_font_quality = SELECTED_FONT_QUALITY_LCD;

            SDL_Color solid_text_color = (SDL_Color){0,0,0,0};
            SDL_Color selected_text_color = (SDL_Color){255,255,255,0};
            SDL_Color selected_text_background_color = (SDL_Color){0,0,255,0};

        };


        //max_chars must have lower size than text_data_in_out 'cause in text_data_in_out needed is extra space for char '\0';
        TextBoxClass(uint32_t max_chars)
        {
            this->max_chars = max_chars;
        }

        ~TextBoxClass()
        {
            if(this->main_surface!=NULL) SDL_FreeSurface(this->main_surface);
            if(this->main_texture!=NULL) SDL_DestroyTexture(this->main_texture);
        }

        //--DANGEROUS--
        //event - variable filled with actual events; typically you will re-pass this variable after
        //passing it first through the SDL_PollEvent function;
        //this container must have greater size than passed value to constructor "max_chars" 'cause needed is extra space for char '\0'
        //
        //text_data_in_out - main data container where all the text is keeped;
        //
        //variables - stays for filled structure with variables
        //
        //do_display_textures - set 0 if u want to not display text texture and beam texture; otherwise set 1;
        //this variable helps in situation when u have transparent textures and u have to run this function more than
        //once at one frame;
        //
        //text_data_in_out must have '\0' at the end of string; otherwise it will crash;
        //changing rect_beam.x dynamically causing bugs and should not be changed;
        void TextBox(SDL_Event* event,uint8_t* text_data_in_out,TextBoxStructure_settings* variables,bool do_display_textures);

        //if returns 1 then data in textbox should be safed to file/when focus of the textbox has been lost (help tool)
        //else returns 0
        uint8_t do_data_should_be_safed() const;

        //--DANGEROUS--
        //returns pointer to the texture of text;
        //this texture shouldn't be freed without refilled 'cause TextBox() frees and allocates memory of this variables;
        //if u leaves this variables withouat allocated memory then program may eventually crash;
        //variable will be filled with data after first enter to the function TextBox();
        //variable will be updated with each run of function TextBox();
        SDL_Texture* get_text_texture();

        //returns pointer to the rect of text texture;
        //variable will be filled with data after first enter to the function TextBox();
        SDL_Rect* get_rect_of_text_texture();

        //returns pointer to the texture of beam;
        //variable will be filled with data after first enter to the function TextBox();
        SDL_Texture* get_beam_texture();

        //--DANGEROUS--
        //returns pointer to the rect of beam texture;
        //variables in this structure shouldn't be changed, especially .x 'cause it'll 'causing bugs;
        //if u want change .y .w .h of this rect then u should do this through function "update_textbox"
        //variable will be filled with data after first enter to the function TextBox();
        //variable will be updated with each run of function TextBox();
        SDL_Rect* get_rect_of_beam_texture();

        //returns 1 if beam texture should be showed for animation purpose or returns 0 if not;
        //variable will be updated with each run of function TextBox();
        bool should_beam_be_showed_or_not() const;

        //returns 0 if cursor should stay "arrow";
        //otherwise returns 1 if cursor should be "beam";
        //variable will be updated with each run of function TextBox();
        bool should_cursor_be_arrow_or_beam() const;

        //updates textures, positions, etc. except rect_beam.x 'cause it's causing ugly visual bugs;
        //run this function if u changed something in ur struct "TextBoxStructure_settings" or in ur data "text_data_in_out";
        void update_textbox();

        //changes limit of allowed chars;
        void set_max_chars(uint32_t limit_of_chars);

        //returns width of text texture;
        uint32_t get_width_of_text_texture();

};

#endif



