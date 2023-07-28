# SDL_TextBox

![printscreen](https://github.com/StandardUserConstr/SDL_TextBox/assets/127525288/3466142b-0cad-4465-919b-75f1e108f296)

documentation of function TextBoxClass::TextBox() and other functions are in header SDL_TextBox.h;

Function "TextBox" only can handle letters between 0 - 255 in ASCII representation; Function that can handle full UTF8 or UNICODE should be created separately ('cause everything was design to handle a characters 1 byte long) and doesn't exist right now;

1. include header SDL_TextBox.h (it is also required to include the SDL.h and SDL_TTF.h libraries before using SDL_TextBox.h);
2. Create object of the class TextBoxClass and in constructor put number of maximum chars that you want to have in ur textbox;
3. allocate data for second argument of function TextBoxClass::TextBox() which is greater minimum by 1 than number you put into
constructor of class TextBoxClass 'cause of **char '\0' that you MUST add into your allocated data where u want ends the string;**
4. Create structure TextBoxClass::TextBoxStructure_settings and fill it with data (detailed description of this operation is below);
5. Fill variable of SDL_Event with actual events using for example SDL_PollEvent() and pass this variable to the function TextBoxClass::TextBox();

code:

    //1
    #include "SDL_TextBox.h"
    
    //2
    int max_chars_in_textbox = 32;
    TextBoxClass object(max_chars_in_textbox);
    
    //3
    uint8_t* data = (uint8_t*)malloc(max_chars_in_textbox+1);
    data[0] = '\0'; //**IMPORTANT**

    //4
    TextBoxClass::TextBoxStructure_settings object_settings;
    
                                                                                          //position and max size of the created text texture;
                                                                                          //decide where u want to show texture of the data in ur render;
    object_settings.x = 14;
    object_settings.y = 36;
    object_settings0.w = 240;
    object_settings.h = 21;
    
                                                                                         //padding of the actual text in ur text texture;
    object_settings.padding_up = 4;
    object_settings.padding_left = 4;
   
    object_settings.beam = IMG_LoadTexture(render,"beam_texture.png");                   //beam texture;
    object_settings.main_font = TTF_OpenFont("aria.ttf",13);                             //font;
    object_settings.rect_beam = (SDL_Rect){17,39,1,15};                                  //rect of the beam texture;
    object_settings.render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);   //render where u want to paste texture of beam and text texture;
   
    object_settings.mode = TextBoxClass::MODE_ALL;                                       //choose mode which decide which letter is allowed to be in ur textbox;
                                                                                         //MODE_LETTERS || MODE_NUMBERS || MODE_ALL || MODE_LETTERS_WITH_SYMBOLS
                                                                                         || MODE_NUMBERS_WITH_SYMBOLS || MODE_SMALL_LETTERS
                                                                                         ||MODE_SMALL_LETTERS_WITH_NUMBERS;

    object_settings.extra_char_allowed_to_mode = 0;                                      //extra char that is allowed to be in ur textbox that isn't in ur mode;
    
    object_settings.beam_delay_in_ms = 500;                                              //animation of show up and hiding ur beam texture in miliseconds;
    object_settings.is_space_allowed = 1;                                                //decides do space is allowed in ur mode or not;
    object_settings.double_click_max_delay_ms = 500;                                     //decide how fast must be ur double click to select the text;
    
    object_settings.solid_text_color = {0,0,0,0};                                        //color of the text;
    object_settings.selected_text_color = {255,255,255,0};                               //color of the selected text;
    object_settings.selected_text_background_color = {0,0,255,0};                        //background color of the selected text;
    
    object_settings.solid_font_quality = TextBoxClass::SOLID_FONT_QUALITY_STANDARD;      //quality mode for render text: SOLID_FONT_QUALITY_STANDARD | SOLID_FONT_QUALITY_BLENDED;
    object_settings.selected_font_quality = TextBoxClass::SELECTED_FONT_QUALITY_LCD;     //quality mode for render selected text: SELECTED_FONT_QUALITY_LCD | SELECTED_FONT_QUALITY_SHADED;
    
    
    //5
    while(1)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        object.TextBox(&event,data,&object_settings,true);
    }
