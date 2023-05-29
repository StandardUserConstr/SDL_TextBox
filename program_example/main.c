#include "SDL_TextBox.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>

#define printf_error(string) fprintf(stderr,string)

struct Global_Variables
{
     SDL_Cursor* arrow_cursor;
     SDL_Cursor* beam_cursor;
     TTF_Font* main_ttf;
     SDL_Window* window;
     SDL_Renderer* render;
};

int init_everything(Global_Variables* variables);
void free_everything(Global_Variables* variables);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Global_Variables global_variables;
    if(init_everything(&global_variables)!=0) return 1;

    SDL_Texture* background0 = IMG_LoadTexture(global_variables.render,"sprites&fonts/background_image.png");
    SDL_Rect rect_background0 = {14,36,240,21};
    SDL_Texture* beam_for_texbox = IMG_LoadTexture(global_variables.render,"sprites&fonts/beam_image.png");
    bool close_program = 0;

    int data_size = 25;
    TextBoxClass textbox_object0(data_size-1); //data size must be greater than max_chars in textbox 'cause needed is extra place for char '\0';
    char* textbox_object0_data = (char*)malloc(data_size);

    TextBoxClass::TextBoxStructure_settings textbox_object0_settings;

    textbox_object0_data[0] = 't';
    textbox_object0_data[1] = 'e';
    textbox_object0_data[2] = 's';
    textbox_object0_data[3] = 't';
    textbox_object0_data[4] = '\0';

    textbox_object0_settings.x = rect_background0.x;
    textbox_object0_settings.y = rect_background0.y;
    textbox_object0_settings.w = rect_background0.w;
    textbox_object0_settings.h = rect_background0.h;
    textbox_object0_settings.padding_left = 4;
    textbox_object0_settings.padding_up = 4;

    textbox_object0_settings.render = global_variables.render;
    textbox_object0_settings.main_font = global_variables.main_ttf;
    textbox_object0_settings.beam = beam_for_texbox;
    textbox_object0_settings.rect_beam = (SDL_Rect){17,39,1,15};

    textbox_object0_settings.mode = TextBoxClass::MODE_NUMBERS;
    textbox_object0_settings.is_space_allowed = 0;
    textbox_object0_settings.extra_char_allowed_to_mode = '.';

    textbox_object0_settings.solid_text_color = {0,0,0,0};
    textbox_object0_settings.selected_text_color = {255,255,255,0};
    textbox_object0_settings.selected_text_background_color = {0,0,255,0};
    textbox_object0_settings.solid_font_quality = TextBoxClass::SOLID_FONT_QUALITY_BLENDED;
    textbox_object0_settings.selected_font_quality = TextBoxClass::SELECTED_FONT_QUALITY_SHADED;

    textbox_object0_settings.beam_delay_in_ms = 500;
    textbox_object0_settings.double_click_max_delay_ms = 500;

    int counter = 0;
    while(close_program==0)
    {

    //display background for textbox
        SDL_RenderClear(global_variables.render);
        SDL_RenderCopy(global_variables.render,background0,NULL,&rect_background0);

    //animation
        uint8_t r,g,b = 255;
        if(counter%5==0)
        {
            r = rand()%255;
            g = rand()%255;
            b = rand()%255;
        }
        counter++;
        if(counter==6) counter = 1;
        textbox_object0_settings.selected_text_background_color = (SDL_Color){r,g,b,0};

        textbox_object0.update_textbox(); //you have to run function "update_textbox()" after changing something in TextBoxStructure_settings or in your text_data_in_out;

    //textbox & event handle
        SDL_Event event;
        textbox_object0.TextBox(&event,(uint8_t*)textbox_object0_data,&textbox_object0_settings,false);
                                //in loop while(SDL_PollEvent() function Textbox can be not executed, and thats why i executing this function for once here
                                //'cause i want to update content of this function every frame;
        while(SDL_PollEvent(&event))
        {

            switch(event.type)
            {
                case SDL_QUIT:
                    close_program = 1;
                break;

                default:
                    break;
            }
            textbox_object0.TextBox(&event,(uint8_t*)textbox_object0_data,&textbox_object0_settings,false);
        }

        if(textbox_object0.should_cursor_be_arrow_or_beam()==1) SDL_SetCursor(global_variables.beam_cursor);
        else SDL_SetCursor(global_variables.arrow_cursor);

        SDL_RenderCopy(global_variables.render,textbox_object0.get_text_texture(),NULL,textbox_object0.get_rect_of_text_texture());
        SDL_RenderCopy(global_variables.render,textbox_object0.get_beam_texture(),NULL,textbox_object0.get_rect_of_beam_texture());

    //display render and freeze program for (1000/30)ms
        SDL_RenderPresent(global_variables.render);
        SDL_Delay(1000/30);
    }
    SDL_DestroyTexture(background0);
    SDL_DestroyTexture(beam_for_texbox);

    free_everything(&global_variables);
    return 0;
}

int init_everything(Global_Variables* variables)
{
    if (SDL_Init(SDL_INIT_EVENTS|SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf_error("Initialization SDL2 failed");
        return 1;
    }

    variables->arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    if(variables->arrow_cursor==NULL)
    {
        printf_error("failed to load system cursor \"arrow\"");
        return 1;
    }

    variables->beam_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    if(variables->beam_cursor==NULL)
    {
        printf_error("failed to load system cursor \"ibeam\"");
        return 1;
    }

    if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)!=(IMG_INIT_JPG|IMG_INIT_PNG))
    {
        printf_error("Initialization SDL2_image failed");
        return 1;

    }
    if(TTF_Init()!=0)
    {
        printf_error("Initialization SDL2_ttf failed");
        return 1;
    }
    variables->main_ttf = TTF_OpenFont("sprites&fonts/arial.ttf",13);
    if(variables->main_ttf==NULL)
    {
        printf_error("Cannot load: \"files/fonts/W95FA.otf\"failed");
        return 1;
    }

    variables->window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,384,240,SDL_WINDOW_SHOWN);

    if(variables->window==NULL)
    {
        printf_error("Creating window failed");
        return 1;
    }

    variables->render = SDL_CreateRenderer(variables->window, -1, SDL_RENDERER_ACCELERATED);
    if(variables->render==NULL)
    {
        printf_error("Creating render failed");
        return 1;
    }
    SDL_SetRenderDrawColor(variables->render,192,199,200,255);
    return 0;
}

void free_everything(Global_Variables* variables)
{
    SDL_FreeCursor(variables->arrow_cursor);
    SDL_FreeCursor(variables->beam_cursor);
    SDL_DestroyRenderer(variables->render);
    SDL_DestroyWindow(variables->window);
    TTF_CloseFont(variables->main_ttf);
    TTF_Quit();
    IMG_Quit();
    return;
}
