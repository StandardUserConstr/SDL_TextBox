#include "SDL_TextBox.h"

void TextBoxClass::combine_surfaces(uint32_t x,uint32_t y,SDL_Surface* src,SDL_Surface* dst)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_BlitSurface(src,NULL,dst,&rect);
    return;
}

void TextBoxClass::TextBox(SDL_Event* event,uint8_t* text_data_in_out,TextBoxStructure_settings* variables,bool do_display_textures)
{
    static const uint8_t MOUSE_BUTTON_HOLD = 2;
    static const uint8_t MOUSE_CAPTURED = 1;
    static const uint8_t MOUSE_NONE = 0;

    this->do_data_should_be_safed_variable = 0;

    if(this->first_enter==0)
    {
        uint32_t counter_first = 0;
        for(counter_first; text_data_in_out[counter_first]!='\0'; counter_first++){}
        if(counter_first!=this->how_many_chars_in_buffor)
        {
                this->at_what_position_is_beam = counter_first;
                this->how_many_chars_in_buffor = counter_first;

                int w_beam,h_beam;
                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                this->rect_beam.x = this->base_beam_x+w_beam+1;
                this->width_of_text_texture = w_beam;
                this->select_start_position_x = counter_first;
                this->select_end_position_x = counter_first;
        }
    }
    if(this->first_enter==1)
    {
        this->x = variables->x;
        this->y = variables->y;
        this->w = variables->w;
        this->h = variables->h;
        this->padding_left = variables->padding_left;
        this->padding_up = variables->padding_up;
        this->beam = variables->beam;
        this->rect_beam = variables->rect_beam;
        this->render = variables->render;
        this->main_font = variables->main_font;
        this->beam_delay_in_ms = variables->beam_delay_in_ms;
        this->mode = variables->mode;
        this->extra_char_allowed_to_mode = variables->extra_char_allowed_to_mode;
        this->is_space_allowed = variables->is_space_allowed;
        this->double_click_max_delay_ms = variables->double_click_max_delay_ms;
        this->selected_text_color = variables->selected_text_color;
        this->solid_text_color = variables->solid_text_color;
        this->selected_text_background_color = variables->selected_text_background_color;
        this->solid_font_quality = variables->solid_font_quality;
        this->selected_font_quality = variables->selected_font_quality;

        this->first_enter = 0;
        uint32_t i = 0;
        for(i; text_data_in_out[i]!='\0'; i++){}
        this->how_many_chars_in_buffor = i;
        this->at_what_position_is_beam = i;

        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_Surface* text0;
        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
        else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);

        this->combine_surfaces(0,0,text0,this->main_surface);
        SDL_FreeSurface(text0);

        int w_beam,h_beam;
        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
        this->base_beam_x = this->rect_beam.x;
        this->rect_beam.x = this->base_beam_x+w_beam+1;
        this->width_of_text_texture = w_beam;

        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

        this->main_texture_rect.x = this->x+this->padding_left;
        this->main_texture_rect.y = this->y+this->padding_up;
        this->main_texture_rect.w = this->w;
        this->main_texture_rect.h = this->h;

    }
    else if(this->updated_textbox==1)
    {
        this->updated_textbox = 0;
        this->x = variables->x;
        this->y = variables->y;
        this->w = variables->w;
        this->h = variables->h;
        this->padding_left = variables->padding_left;
        this->padding_up = variables->padding_up;
        this->beam = variables->beam;
        this->rect_beam.y = variables->rect_beam.y;
        this->rect_beam.w = variables->rect_beam.w;
        this->rect_beam.h = variables->rect_beam.h;
        this->render = variables->render;
        this->main_font = variables->main_font;
        this->beam_delay_in_ms = variables->beam_delay_in_ms;
        this->mode = variables->mode;
        this->extra_char_allowed_to_mode = variables->extra_char_allowed_to_mode;
        this->is_space_allowed = variables->is_space_allowed;
        this->double_click_max_delay_ms = variables->double_click_max_delay_ms;
        this->selected_text_color = variables->selected_text_color;
        this->solid_text_color = variables->solid_text_color;
        this->selected_text_background_color = variables->selected_text_background_color;
        this->solid_font_quality = variables->solid_font_quality;
        this->selected_font_quality = variables->selected_font_quality;

        if(this->select_start_position_x!=this->select_end_position_x)
        {
            int w_beam,h_beam;

            SDL_Surface* first_part = NULL;
            uint32_t first_part_width = 0;

            SDL_Surface* second_part = NULL;
            uint32_t second_part_width = 0;

            uint32_t third_part_width = 0;
            SDL_Surface* third_part = NULL;

            if(this->select_end_position_x>this->select_start_position_x)
            {
            //from left to right

            //first
                if(this->select_start_position_x!=0)
                {
                    uint8_t* first_part_data = (uint8_t*)malloc(this->select_start_position_x+1);
                    uint32_t i = 0;
                    for(; i!=this->select_start_position_x ; i++) first_part_data[i] = text_data_in_out[i];
                    first_part_data[i] = '\0';
                    TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                    first_part_width = w_beam;
                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                    else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                    free(first_part_data);
                }
            //second
                uint8_t const_char_data[2] = {'\0','\0'};

                uint8_t* second_part_data;
                second_part_data = (uint8_t*)malloc(this->select_end_position_x-this->select_start_position_x+1);
                for(uint32_t i = this->select_start_position_x,k = 0; ; i++,k++)
                {
                    if(k==this->select_end_position_x-this->select_start_position_x)
                    {
                        second_part_data[k] = '\0';
                        break;
                    }
                    second_part_data[k] = text_data_in_out[i];
                }

                TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                second_part_width = w_beam;
                if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                free(second_part_data);

            //third && setting beam position
                if(this->select_end_position_x!=this->how_many_chars_in_buffor)
                {
                    uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-this->select_end_position_x+1);
                    for(uint32_t i = select_end_position_x,k = 0; ; i++,k++)
                    {
                        if(i==this->how_many_chars_in_buffor)
                        {
                            third_part_data[k] = '\0';
                            break;
                        }
                        third_part_data[k] = text_data_in_out[i];
                    }
                    TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                    third_part_width = w_beam;

                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                    else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                    free(third_part_data);
                }
            }
            else
            {
            //from right to left

            //first
                if(this->select_end_position_x!=0)
                {
                    uint8_t* first_part_data = (uint8_t*)malloc(this->select_end_position_x+1);
                    uint32_t i = 0;
                    for(; i!=this->select_end_position_x ; i++) first_part_data[i] = text_data_in_out[i];
                    first_part_data[i] = '\0';
                    TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                    first_part_width = w_beam;
                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                    else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                    free(first_part_data);
                }
            //second
                uint8_t const_char_data[2] = {'\0','\0'};

                uint8_t* second_part_data;
                second_part_data = (uint8_t*)malloc(this->select_start_position_x-this->select_end_position_x+1);
                for(uint32_t i = this->select_end_position_x,k = 0; ; i++,k++)
                {
                    if(k==this->select_start_position_x-this->select_end_position_x)
                    {
                        second_part_data[k] = '\0';
                        break;
                    }
                    second_part_data[k] = text_data_in_out[i];
                }

                TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                second_part_width = w_beam;
                if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                free(second_part_data);

            //third
                if(this->select_start_position_x!=this->how_many_chars_in_buffor)
                {
                    uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-this->select_start_position_x+1);
                    for(uint32_t i = select_start_position_x,k = 0; ; i++,k++)
                    {
                        if(i==this->how_many_chars_in_buffor)
                        {
                            third_part_data[k] = '\0';
                            break;
                        }
                        third_part_data[k] = text_data_in_out[i];
                    }
                    TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                    third_part_width = w_beam;

                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                    else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                    free(third_part_data);
                }
            }
        //combine
            SDL_FreeSurface(this->main_surface);
            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
            if(first_part!=NULL)
            {
                this->combine_surfaces(0,0,first_part,this->main_surface);
                SDL_FreeSurface(first_part);
                if(second_part!=NULL)
                {
                    this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                    SDL_FreeSurface(second_part);
                    if(third_part!=NULL)
                    {
                        this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                        SDL_FreeSurface(third_part);
                    }
                }
                else if(third_part!=NULL)
                {
                    this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                    SDL_FreeSurface(third_part);
                }


            }
            else if(second_part!=NULL)
            {
                this->combine_surfaces(0,0,second_part,this->main_surface);
                SDL_FreeSurface(second_part);
                if(third_part!=NULL)
                {
                    this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                    SDL_FreeSurface(third_part);
                }

            }
            else
            {
                this->combine_surfaces(0,0,third_part,this->main_surface);
                SDL_FreeSurface(third_part);

            }
            SDL_DestroyTexture(this->main_texture);
            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

        }
        else
        {
            SDL_FreeSurface(this->main_surface);
            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
            SDL_Surface* text0;
            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
            else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
            this->combine_surfaces(0,0,text0,this->main_surface);
            SDL_FreeSurface(text0);
            SDL_DestroyTexture(this->main_texture);
            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
        }

        this->main_texture_rect.x = this->x+this->padding_left;
        this->main_texture_rect.y = this->y+this->padding_up;
        this->main_texture_rect.w = this->w;
        this->main_texture_rect.h = this->h;
    }

    switch(event->type)
    {
        case SDL_MOUSEMOTION:
        {
            if((event->motion.x>=this->x && event->motion.x<=this->x+this->w) && (event->motion.y>=this->y && event->motion.y<=this->y+this->h))
            {
                if(this->mouse_state!=MOUSE_BUTTON_HOLD) this->mouse_state = MOUSE_CAPTURED;
                this->cursor_display = 1;
            }
            else
            {
                if(this->mouse_state!=MOUSE_BUTTON_HOLD) this->mouse_state = MOUSE_NONE;
                this->cursor_display = 0;
            }

            if(this->mouse_state==MOUSE_BUTTON_HOLD)
            {
                if(this->select_base_mouse_x-event->motion.x<0)
                {
                //right
                    uint32_t difference = SDL_abs(this->select_base_mouse_x-event->motion.x);
                    int w_beam,h_beam;

                //first
                    SDL_Surface* first_part = NULL;
                    uint32_t first_part_width = 0;
                    if(this->select_start_position_x!=0)
                    {
                        uint8_t* first_part_data = (uint8_t*)malloc(this->select_start_position_x+1);
                        uint32_t i = 0;
                        for(; i!=this->select_start_position_x ; i++) first_part_data[i] = text_data_in_out[i];
                        first_part_data[i] = '\0';
                        TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                        first_part_width = w_beam;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                        else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                        free(first_part_data);
                    }

                //second
                    SDL_Surface* second_part = NULL;
                    uint8_t const_char_data[2] = {'\0','\0'};
                    uint32_t how_many_chars_in_second_part = 0;
                    uint32_t second_part_width = 0;

                    if(this->select_start_position_x!=this->how_many_chars_in_buffor)
                    {
                        //faster but with bug
                        /*for(uint32_t i = this->select_start_position_x; ; i++)
                        {
                            const_char_data[0] = text_data_in_out[i];
                            TTF_SizeUTF8(this->main_font,(const char*)const_char_data,&w_beam,&h_beam);
                            if(difference>=w_beam)
                            {
                                difference-=w_beam;
                            }
                            else
                            {
                                if(difference<(w_beam/2)) this->select_end_position_x = i;
                                else this->select_end_position_x = i+1;
                                break;
                            }
                            if(i==this->how_many_chars_in_buffor)
                            {
                                this->select_end_position_x = this->how_many_chars_in_buffor;
                                break;
                            }
                        }*/

                        uint8_t* data_check_difference = (uint8_t*)malloc(this->how_many_chars_in_buffor+1);
                        for(uint32_t i = this->select_start_position_x,j = 0; ; i++,j++)
                        {
                            data_check_difference[j] = text_data_in_out[i];
                            data_check_difference[j+1] = '\0';
                            TTF_SizeUTF8(this->main_font,(const char*)data_check_difference,&w_beam,&h_beam);
                            if(difference<w_beam)
                            {
                                int fast_w_beam,fast_h_beam;
                                const_char_data[0] = data_check_difference[j];
                                TTF_SizeUTF8(this->main_font,(const char*)const_char_data,&fast_w_beam,&fast_h_beam);
                                if(difference<(w_beam-(fast_w_beam/2))) this->select_end_position_x = i;
                                else this->select_end_position_x = i+1;
                                break;
                            }
                            if(i==this->how_many_chars_in_buffor)
                            {
                                this->select_end_position_x = this->how_many_chars_in_buffor;
                                break;
                            }
                        }
                        free(data_check_difference);

                        uint8_t* second_part_data = (uint8_t*)malloc(this->select_end_position_x-this->select_start_position_x+1);
                        for(uint32_t i = this->select_start_position_x,k = 0; ; i++,k++)
                        {
                            if(k==this->select_end_position_x-this->select_start_position_x)
                            {
                                second_part_data[k] = '\0';
                                break;
                            }
                            how_many_chars_in_second_part++;
                            second_part_data[k] = text_data_in_out[i];
                        }

                        if(how_many_chars_in_second_part!=0)
                        {
                            TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                            second_part_width = w_beam;
                            if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                            else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                            #ifdef TEXBOX_DEBUG
                            printf("TextBoxClass::TextBox:\n");
                            printf("    select_start=%zd\n    select_end=%zd\n",this->select_start_position_x,this->select_end_position_x);
                            printf("        text=%s\n\n",second_part_data);
                            #endif
                        }
                        free(second_part_data);
                    }
                    else this->select_end_position_x = this->how_many_chars_in_buffor;


                //third && setting beam position
                    uint32_t third_part_width = 0;
                    SDL_Surface* third_part = NULL;
                    if(this->select_end_position_x!=this->how_many_chars_in_buffor||this->select_start_position_x!=how_many_chars_in_buffor)
                    {
                        uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-this->select_end_position_x+1);
                        for(uint32_t i = select_end_position_x,k = 0; ; i++,k++)
                        {
                            if(i==this->how_many_chars_in_buffor)
                            {
                                third_part_data[k] = '\0';
                                break;
                            }
                            third_part_data[k] = text_data_in_out[i];
                        }
                        TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                        third_part_width = w_beam;

                        this->rect_beam.x = this->base_beam_x+first_part_width+second_part_width+1;
                        this->at_what_position_is_beam = this->select_end_position_x;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                        else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                        free(third_part_data);
                    }
                    else
                    {
                        this->at_what_position_is_beam = how_many_chars_in_buffor;
                        this->rect_beam.x = this->base_beam_x+first_part_width+second_part_width+1;

                    }

                //combine
                    SDL_FreeSurface(this->main_surface);
                    this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                    if(first_part!=NULL)
                    {
                        this->combine_surfaces(0,0,first_part,this->main_surface);
                        SDL_FreeSurface(first_part);
                        if(second_part!=NULL)
                        {
                            this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                            SDL_FreeSurface(second_part);
                            if(third_part!=NULL)
                            {
                                this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                                SDL_FreeSurface(third_part);
                            }
                        }
                        else if(third_part!=NULL)
                        {
                            this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                            SDL_FreeSurface(third_part);
                        }


                    }
                    else if(second_part!=NULL)
                    {
                        this->combine_surfaces(0,0,second_part,this->main_surface);
                        SDL_FreeSurface(second_part);
                        if(third_part!=NULL)
                        {
                            this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                            SDL_FreeSurface(third_part);
                        }

                    }
                    else
                    {
                        this->combine_surfaces(0,0,third_part,this->main_surface);
                        SDL_FreeSurface(third_part);

                    }
                    this->width_of_text_texture = first_part_width+second_part_width+third_part_width;
                    SDL_DestroyTexture(this->main_texture);
                    this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);



                }
                else if(this->select_base_mouse_x-event->motion.x>0&&((event->motion.x<(this->width_of_text_texture+this->main_texture_rect.x))||(this->select_end_position_x!=this->select_start_position_x)))
                {
                //left
                    uint32_t difference = this->select_base_mouse_x-event->motion.x;
                    int w_beam,h_beam;

                //third
                    SDL_Surface* third_part = NULL;
                    uint32_t third_part_width = 0;
                    if(this->select_start_position_x!=this->how_many_chars_in_buffor)
                    {
                        uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-this->select_start_position_x+1);
                        uint32_t k = 0;
                        for(uint32_t i = this->how_many_chars_in_buffor; i!=(this->how_many_chars_in_buffor-(this->how_many_chars_in_buffor-this->select_start_position_x)); i--,k++)
                        {third_part_data[k] = text_data_in_out[this->select_start_position_x+k];}

                        third_part_data[k] = '\0';
                        TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                        third_part_width = w_beam;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                        else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                        free(third_part_data);
                    }


                //second
                    SDL_Surface* second_part = NULL;
                    uint8_t const_char_data[2] = {'\0','\0'};
                    uint32_t how_many_chars_in_second_part = 0;
                    uint32_t second_part_width = 0;

                    if(this->select_start_position_x!=0)
                    {
                        //faster but with bug
                        /*for(uint32_t i = this->select_start_position_x-1; ; i--)
                        {
                            const_char_data[0] = text_data_in_out[i];
                            TTF_SizeUTF8(this->main_font,(const char*)const_char_data,&w_beam,&h_beam);
                            if(difference>=w_beam)
                            {
                                difference-=w_beam;
                            }
                            else
                            {
                                if(difference<(w_beam/2)) this->select_end_position_x = i+1;
                                else this->select_end_position_x = i;
                                break;
                            }
                            if(i==0)
                            {
                                this->select_end_position_x = 0;
                                break;
                            }
                        }*/

                        uint8_t* data_check_difference = (uint8_t*)malloc(this->how_many_chars_in_buffor+1);
                        for(uint32_t i = this->select_start_position_x-1,j = 0; ; i--,j++)
                        {
                            data_check_difference[j] = text_data_in_out[i];
                            data_check_difference[j+1] = '\0';
                            TTF_SizeUTF8(this->main_font,(const char*)data_check_difference,&w_beam,&h_beam);
                            if(difference<w_beam)
                            {
                                int fast_w_beam,fast_h_beam;
                                const_char_data[0] = data_check_difference[j];
                                TTF_SizeUTF8(this->main_font,(const char*)const_char_data,&fast_w_beam,&fast_h_beam);
                                if(difference<(w_beam-(fast_w_beam/2))) this->select_end_position_x = i+1;
                                else this->select_end_position_x = i;
                                break;
                            }
                            if(i==0)
                            {
                                this->select_end_position_x = 0;
                                break;
                            }
                        }
                        free(data_check_difference);

                        uint8_t* second_part_data = (uint8_t*)malloc(this->select_start_position_x-this->select_end_position_x+1);
                        for(uint32_t i = this->select_end_position_x,k = 0; ; i++,k++)
                        {
                            if(k==this->select_start_position_x-this->select_end_position_x)
                            {
                                second_part_data[k] = '\0';
                                break;
                            }
                            how_many_chars_in_second_part++;
                            second_part_data[k] = text_data_in_out[i];
                        }

                        if(how_many_chars_in_second_part!=0)
                        {
                            TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                            second_part_width = w_beam;
                            if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                            else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                            #ifdef TEXBOX_DEBUG
                            printf("TextBoxClass::TextBox:\n");
                            printf("    select_start=%zd\n    select_end=%zd\n",this->select_start_position_x,this->select_end_position_x);
                            printf("        text=%s\n\n",second_part_data);
                            #endif
                        }
                        free(second_part_data);
                    }
                    else this->select_end_position_x = 0;

                //first && setting beam position
                    uint32_t first_part_width = 0;
                    SDL_Surface* first_part = NULL;
                    if(this->select_end_position_x!=0||this->select_start_position_x!=0)
                    {
                        uint8_t* first_part_data = (uint8_t*)malloc(this->select_end_position_x+1);
                        for(uint32_t i = 0; ; i++)
                        {
                            if(i==this->select_end_position_x)
                            {
                                first_part_data[i] = '\0';
                                break;
                            }
                            first_part_data[i] = text_data_in_out[i];
                        }
                        TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                        first_part_width = w_beam;

                        this->rect_beam.x = this->base_beam_x+w_beam+1;
                        this->at_what_position_is_beam = this->select_end_position_x;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                        else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                        free(first_part_data);
                    }
                    else
                    {
                        this->at_what_position_is_beam = 0;
                        this->rect_beam.x = this->base_beam_x+1;

                    }


                //combine
                    SDL_FreeSurface(this->main_surface);
                    this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                    if(first_part!=NULL)
                    {
                        this->combine_surfaces(0,0,first_part,this->main_surface);
                        SDL_FreeSurface(first_part);
                        if(second_part!=NULL)
                        {
                            this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                            SDL_FreeSurface(second_part);
                            if(third_part!=NULL)
                            {
                                this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                                SDL_FreeSurface(third_part);
                            }
                        }
                        else if(third_part!=NULL)
                        {
                            this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                            SDL_FreeSurface(third_part);
                        }


                    }
                    else if(second_part!=NULL)
                    {
                        this->combine_surfaces(0,0,second_part,this->main_surface);
                        SDL_FreeSurface(second_part);
                        if(third_part!=NULL)
                        {
                            this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                            SDL_FreeSurface(third_part);
                        }

                    }
                    else
                    {
                        this->combine_surfaces(0,0,third_part,this->main_surface);
                        SDL_FreeSurface(third_part);

                    }
                    this->width_of_text_texture = first_part_width+second_part_width+third_part_width;
                    SDL_DestroyTexture(this->main_texture);
                    this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);


                }
            }

        }break;

        case SDL_MOUSEBUTTONDOWN:
            {
                if(this->mouse_state==MOUSE_CAPTURED)
                {
                    this->mouse_state = MOUSE_BUTTON_HOLD;
                    this->focus = 1;
                    this->beam_timer0 = SDL_GetTicks();
                    this->beam_timer1 = this->beam_timer0;
                    this->beam_animation_direction = 0;
                    this->cursor_display = 1;

                    int w_beam,h_beam;
                    uint32_t total_lenght = this->base_beam_x+1;
                    uint8_t buffor = '\0';
                    for(uint32_t i = 0; ; i++)
                    {
                        if(total_lenght>=event->motion.x)
                        {
                            this->at_what_position_is_beam = i;
                            this->rect_beam.x = total_lenght;
                            break;
                        }
                        if(text_data_in_out[i]=='\0')
                        {
                            this->at_what_position_is_beam = this->how_many_chars_in_buffor;
                            this->rect_beam.x = total_lenght;
                            break;
                        }
                        buffor = text_data_in_out[i+1];
                        text_data_in_out[i+1] = '\0';
                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                        text_data_in_out[i+1] = buffor;
                        total_lenght = this->base_beam_x+w_beam+1;
                    }
                    this->select_base_mouse_x = total_lenght;
                    this->width_of_text_texture = total_lenght-(this->base_beam_x+1);

                    if(this->select_start_position_x!=this->select_end_position_x)
                    {
                        SDL_FreeSurface(this->main_surface);
                        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_Surface* text0;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        this->combine_surfaces(0,0,text0,this->main_surface);
                        SDL_FreeSurface(text0);
                        SDL_DestroyTexture(this->main_texture);
                        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                    }
                    this->select_start_position_x = this->at_what_position_is_beam;
                    this->select_end_position_x = this->at_what_position_is_beam;
                    #ifdef TEXBOX_DEBUG
                    printf("TextBoxClass::TextBox:\n");
                    printf("    beam_position=%zd\n\n",this->at_what_position_is_beam);
                    #endif

                }
                if(this->mouse_state==MOUSE_BUTTON_HOLD)
                {
                    this->cursor_display = 1;
                    if((this->at_what_position_is_beam==this->double_click_last_beam_position)&&(this->double_click_max_delay_ms>=(SDL_GetTicks()-this->double_click_timer0))&&(this->double_click_enter==1))
                    {
                        this->double_click_enter = 0;
                        if(this->how_many_chars_in_buffor==0) break;

                        uint32_t select_couter;
                        if(this->select_start_position_x==0)
                        {
                            if(text_data_in_out[this->select_start_position_x]!=' ')
                            {
                                for(select_couter = this->select_end_position_x; select_couter!=this->how_many_chars_in_buffor; select_couter++)
                                {
                                    if(text_data_in_out[select_couter]==' ') break;
                                }
                                this->select_end_position_x = select_couter;
                            }
                            else
                            {
                                for(select_couter = this->select_end_position_x; select_couter!=this->how_many_chars_in_buffor; select_couter++)
                                {
                                    if(text_data_in_out[select_couter]!=' ') break;
                                }
                                this->select_end_position_x = select_couter;
                            }
                        }
                        else
                        {
                            if(text_data_in_out[this->select_start_position_x-1]!=' ')
                            {
                                for(select_couter = this->select_end_position_x; select_couter!=this->how_many_chars_in_buffor; select_couter++)
                                {
                                    if(text_data_in_out[select_couter]==' ') break;
                                }
                                this->select_end_position_x = select_couter;

                                for(select_couter = this->select_start_position_x-1; ; select_couter--)
                                {
                                    if(text_data_in_out[select_couter]==' ')
                                    {
                                        select_couter++;
                                        break;
                                    }
                                    if(select_couter==0) break;
                                }
                                this->select_start_position_x = select_couter;
                            }
                            else
                            {
                                for(select_couter = this->select_end_position_x; select_couter!=this->how_many_chars_in_buffor; select_couter++)
                                {
                                    if(text_data_in_out[select_couter]!=' ') break;
                                }
                                this->select_end_position_x = select_couter;

                                for(select_couter = this->select_start_position_x-1; ; select_couter--)
                                {
                                    if(text_data_in_out[select_couter]!=' ')
                                    {
                                        select_couter++;
                                        break;
                                    }
                                    if(select_couter==0) break;
                                }
                                this->select_start_position_x = select_couter;
                            }
                        }




                        this->at_what_position_is_beam = this->select_end_position_x;
                        //printf("start=%d\nend=%d\n",this->select_start_position_x,this->select_end_position_x);
                        //for(uint32_t i = this->select_start_position_x; i<this->select_end_position_x; i++) printf("|%c|",text_data_in_out[i]);

                        int w_beam,h_beam;

                    //first
                        SDL_Surface* first_part = NULL;
                        uint32_t first_part_width = 0;
                        if(this->select_start_position_x!=0)
                        {
                            uint8_t* first_part_data = (uint8_t*)malloc(this->select_start_position_x+1);
                            uint32_t i = 0;
                            for(; i!=this->select_start_position_x ; i++) first_part_data[i] = text_data_in_out[i];
                            first_part_data[i] = '\0';
                            TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                            first_part_width = w_beam;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                            else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                            free(first_part_data);
                        }

                    //second
                        SDL_Surface* second_part = NULL;
                        uint8_t const_char_data[2] = {'\0','\0'};
                        uint32_t second_part_width = 0;

                        uint8_t* second_part_data = (uint8_t*)malloc(this->select_end_position_x-this->select_start_position_x+1);
                        for(uint32_t i = this->select_start_position_x,k = 0; ; i++,k++)
                        {
                            if(k==this->select_end_position_x-this->select_start_position_x)
                            {
                                second_part_data[k] = '\0';
                                break;
                            }
                            second_part_data[k] = text_data_in_out[i];
                        }

                        #ifdef TEXBOX_DEBUG
                        printf("TextBoxClass::TextBox:\n");
                        printf("    select_start=%d\n    select_end=%d\n",this->select_start_position_x,this->select_end_position_x);
                        printf("        text=%s\n\n",second_part_data);
                        #endif

                        TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                        second_part_width = w_beam;
                        if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                        else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                        free(second_part_data);

                    //third && setting beam position
                        uint32_t third_part_width = 0;
                        SDL_Surface* third_part = NULL;
                        if(this->select_end_position_x!=this->how_many_chars_in_buffor)
                        {
                            uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-this->select_end_position_x+1);
                            for(uint32_t i = select_end_position_x,k = 0; ; i++,k++)
                            {
                                if(i==this->how_many_chars_in_buffor)
                                {
                                    third_part_data[k] = '\0';
                                    break;
                                }
                                third_part_data[k] = text_data_in_out[i];
                            }
                            TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                            third_part_width = w_beam;

                            this->rect_beam.x = this->base_beam_x+first_part_width+second_part_width+1;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                            else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                            free(third_part_data);
                        }
                        else
                        {
                            this->rect_beam.x = this->base_beam_x+first_part_width+second_part_width+1;

                        }

                    //combine
                        SDL_FreeSurface(this->main_surface);
                        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        if(first_part!=NULL)
                        {
                            this->combine_surfaces(0,0,first_part,this->main_surface);
                            SDL_FreeSurface(first_part);
                            if(second_part!=NULL)
                            {
                                this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                                SDL_FreeSurface(second_part);
                                if(third_part!=NULL)
                                {
                                    this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                                    SDL_FreeSurface(third_part);
                                }
                            }
                            else if(third_part!=NULL)
                            {
                                this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                                SDL_FreeSurface(third_part);
                            }


                        }
                        else if(second_part!=NULL)
                        {
                            this->combine_surfaces(0,0,second_part,this->main_surface);
                            SDL_FreeSurface(second_part);
                            if(third_part!=NULL)
                            {
                                this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                                SDL_FreeSurface(third_part);
                            }

                        }
                        else
                        {
                            this->combine_surfaces(0,0,third_part,this->main_surface);
                            SDL_FreeSurface(third_part);

                        }
                        SDL_DestroyTexture(this->main_texture);
                        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                    }
                    else
                    {
                        this->double_click_last_beam_position = this->at_what_position_is_beam;
                        this->double_click_timer0 = SDL_GetTicks();
                        this->double_click_enter = 1;
                    }
                }

            }break;

        case SDL_MOUSEBUTTONUP:
            {
                if(this->mouse_state==MOUSE_BUTTON_HOLD)
                {
                    if((event->motion.x>=this->x && event->motion.x<=this->x+this->w) && (event->motion.y>=this->y && event->motion.y<=this->y+this->h))
                    {
                        this->mouse_state = MOUSE_CAPTURED;
                        this->cursor_display = 1;
                    }
                    else
                    {
                        this->mouse_state = MOUSE_NONE;
                        this->cursor_display = 0;
                    }


                }
                else if(this->focus==1&&this->mouse_state==MOUSE_NONE)
                {
                    this->focus = 0;
                    this->mouse_state = MOUSE_NONE;
                    this->cursor_display = 0;


                    if(this->select_end_position_x!=this->select_start_position_x)
                    {
                        this->select_start_position_x = 0;
                        this->select_end_position_x = 0;
                        SDL_FreeSurface(this->main_surface);
                        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_Surface* text0;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        this->combine_surfaces(0,0,text0,this->main_surface);
                        SDL_FreeSurface(text0);
                        SDL_DestroyTexture(this->main_texture);
                        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                        //printf("select has been reseted\n");
                    }

                    //save settings to file when lossing focus
                    this->do_data_should_be_safed_variable = 1;
                }

            }break;

        case SDL_KEYDOWN:
            {
                if(this->focus==0) break;
                #ifdef TEXBOX_DEBUG
                printf("TextBoxClass::TextBox:\n");
                #endif
                SDL_Keymod key_mode = SDL_GetModState();
                char char0 = '\0';
                switch(this->mode)
                {
                    case MODE_LETTERS:
                        {
                            if((event->key.keysym.sym>=97&&event->key.keysym.sym<=122)||event->key.keysym.sym==this->extra_char_allowed_to_mode||event->key.keysym.sym==SDLK_SPACE)
                            {
                                if((key_mode==KMOD_LSHIFT||key_mode==KMOD_CAPS||key_mode==KMOD_RSHIFT)&&(event->key.keysym.sym!=this->extra_char_allowed_to_mode))
                                {
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                    else if(event->key.keysym.sym!=SDLK_SPACE) char0 = event->key.keysym.sym-32;
                                    else char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else
                                {
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                    else char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                            }

                        }break;

                    case MODE_LETTERS_WITH_SYMBOLS:
                        {
                            if((key_mode==KMOD_LSHIFT||key_mode==KMOD_CAPS||key_mode==KMOD_RSHIFT))
                            {
                                if(event->key.keysym.sym>=97&&event->key.keysym.sym<=122)
                                {
                                    char0 = event->key.keysym.sym-32;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else if(event->key.keysym.sym>=51&&event->key.keysym.sym<=53)
                                {
                                    char0= event->key.keysym.sym-16;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else
                                {
                                    switch(event->key.keysym.sym)
                                    {
                                        case 96: char0 = '~'; break;
                                        case 93: char0 = '}'; break;
                                        case 92: char0 = '|'; break;
                                        case 91: char0 = '{'; break;
                                        case 61: char0 = '+'; break;
                                        case 59: char0 = ':'; break;
                                        case 57: char0 = '('; break;
                                        case 56: char0 = '*'; break;
                                        case 55: char0 = '&'; break;
                                        case 54: char0 = '^'; break;
                                        case 50: char0 = '@'; break;
                                        case 49: char0 = '!'; break;
                                        case 48: char0 = ')'; break;
                                        case 47: char0 = '?'; break;
                                        case 46: char0 = '>'; break;
                                        case 45: char0 = '_'; break;
                                        case 44: char0 = '<'; break;
                                        case 39: char0 = '"'; break;
                                        default: break;
                                    }
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed!=0) char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                            }
                            else
                            {
                                if((event->key.keysym.sym>=58&&event->key.keysym.sym<=122)||(event->key.keysym.sym>=32&&event->key.keysym.sym<=47)
                                   ||(event->key.keysym.sym==this->extra_char_allowed_to_mode))
                                {
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                    else char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }

                            }

                        }break;

                    case MODE_NUMBERS:
                        {
                            if((event->key.keysym.sym>=48&&event->key.keysym.sym<=57)||(event->key.keysym.sym==this->extra_char_allowed_to_mode||event->key.keysym.sym==SDLK_SPACE))
                            {
                                if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                else char0 = event->key.keysym.sym;
                                #ifdef TEXBOX_DEBUG
                                printf("    char=%c\n",char0);
                                #endif
                            }

                        }break;

                    case MODE_NUMBERS_WITH_SYMBOLS:
                        {
                            if((key_mode==KMOD_LSHIFT||key_mode==KMOD_CAPS||key_mode==KMOD_RSHIFT))
                            {
                                if(event->key.keysym.sym>=51&&event->key.keysym.sym<=53)
                                {
                                    char0= event->key.keysym.sym-16;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else
                                {
                                    switch(event->key.keysym.sym)
                                    {
                                        case 96: char0 = '~'; break;
                                        case 93: char0 = '}'; break;
                                        case 92: char0 = '|'; break;
                                        case 91: char0 = '{'; break;
                                        case 61: char0 = '+'; break;
                                        case 59: char0 = ':'; break;
                                        case 57: char0 = '('; break;
                                        case 56: char0 = '*'; break;
                                        case 55: char0 = '&'; break;
                                        case 54: char0 = '^'; break;
                                        case 50: char0 = '@'; break;
                                        case 49: char0 = '!'; break;
                                        case 48: char0 = ')'; break;
                                        case 47: char0 = '?'; break;
                                        case 46: char0 = '>'; break;
                                        case 45: char0 = '_'; break;
                                        case 44: char0 = '<'; break;
                                        case 39: char0 = '"'; break;
                                        default: break;
                                    }
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed!=0) char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                            }
                            else
                            {
                                if(((event->key.keysym.sym>=32&&event->key.keysym.sym<=96))||(event->key.keysym.sym==this->extra_char_allowed_to_mode))
                                {
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                    else char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }

                            }

                        }break;

                    case MODE_ALL:
                        {
                            if((key_mode==KMOD_LSHIFT||key_mode==KMOD_CAPS||key_mode==KMOD_RSHIFT))
                            {
                                if(event->key.keysym.sym>=97&&event->key.keysym.sym<=122)
                                {
                                    char0 = event->key.keysym.sym-32;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else if(event->key.keysym.sym>=51&&event->key.keysym.sym<=53)
                                {
                                    char0= event->key.keysym.sym-16;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                                else
                                {
                                    switch(event->key.keysym.sym)
                                    {
                                        case 96: char0 = '~'; break;
                                        case 93: char0 = '}'; break;
                                        case 92: char0 = '|'; break;
                                        case 91: char0 = '{'; break;
                                        case 61: char0 = '+'; break;
                                        case 59: char0 = ':'; break;
                                        case 57: char0 = '('; break;
                                        case 56: char0 = '*'; break;
                                        case 55: char0 = '&'; break;
                                        case 54: char0 = '^'; break;
                                        case 50: char0 = '@'; break;
                                        case 49: char0 = '!'; break;
                                        case 48: char0 = ')'; break;
                                        case 47: char0 = '?'; break;
                                        case 46: char0 = '>'; break;
                                        case 45: char0 = '_'; break;
                                        case 44: char0 = '<'; break;
                                        case 39: char0 = '"'; break;
                                        default: break;
                                    }
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed!=0) char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }
                            }
                            else
                            {
                                if(((event->key.keysym.sym>=32&&event->key.keysym.sym<=122))||(event->key.keysym.sym==this->extra_char_allowed_to_mode))
                                {
                                    if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                    else char0 = event->key.keysym.sym;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    char=%c\n",char0);
                                    #endif
                                }

                            }

                        }break;

                    case MODE_SMALL_LETTERS:
                        {
                            if((event->key.keysym.sym>=97&&event->key.keysym.sym<=122)||event->key.keysym.sym==this->extra_char_allowed_to_mode||event->key.keysym.sym==SDLK_SPACE)
                            {
                                if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                else char0 = event->key.keysym.sym;
                                #ifdef TEXBOX_DEBUG
                                printf("    char=%c\n",char0);
                                #endif
                            }

                        }break;
                    case MODE_SMALL_LETTERS_WITH_NUMBERS:
                        {
                            if((event->key.keysym.sym>=97&&event->key.keysym.sym<=122)||(event->key.keysym.sym>=48&&event->key.keysym.sym<=57)||event->key.keysym.sym==this->extra_char_allowed_to_mode||event->key.keysym.sym==SDLK_SPACE)
                            {
                                if(event->key.keysym.sym==SDLK_SPACE&&this->is_space_allowed==0) break;
                                else char0 = event->key.keysym.sym;
                                #ifdef TEXBOX_DEBUG
                                printf("    char=%c\n",char0);
                                #endif
                            }
                        }break;
                }
                if(key_mode==KMOD_LCTRL||key_mode==KMOD_RCTRL)
                {
                    if(event->key.keysym.sym==SDLK_c&&((this->select_end_position_x-this->select_start_position_x)!=0))
                    {
                        uint8_t* copy_data = (uint8_t*)malloc(SDL_abs(this->select_end_position_x-this->select_start_position_x)+1);
                        if(this->select_start_position_x>this->select_end_position_x)
                        {
                            uint32_t k = 0;
                            for(uint32_t i = this->select_end_position_x; i!=this->select_start_position_x; i++,k++) copy_data[k] = text_data_in_out[i];
                            copy_data[k] = '\0';
                        }
                        else
                        {
                            uint32_t k = 0;
                            for(uint32_t i = this->select_start_position_x; i!=this->select_end_position_x; i++,k++) copy_data[k] = text_data_in_out[i];
                            copy_data[k] = '\0';
                        }

                        if(SDL_SetClipboardText((const char*)copy_data)!=0)
                        {
                            #ifdef TEXBOX_DEBUG
                            fprintf(stderr,"failed copy to clipboard\n");
                            #endif
                        }

                        #ifdef TEXBOX_DEBUG
                        printf("    copied_to_clipboard=%s\n\n",copy_data);
                        #endif

                        free(copy_data);

                        this->beam_timer0 = SDL_GetTicks();
                        this->beam_animation_direction = 0;
                    }
                    else if(event->key.keysym.sym==SDLK_a)
                    {
                        this->select_start_position_x = 0;
                        this->at_what_position_is_beam = how_many_chars_in_buffor;
                        this->select_end_position_x = how_many_chars_in_buffor;

                        SDL_FreeSurface(this->main_surface);
                        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_Surface* text0;
                        if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) text0 = TTF_RenderUTF8_LCD(this->main_font,(const char*)text_data_in_out,this->selected_text_color,this->selected_text_background_color);
                        else text0 = TTF_RenderUTF8_Shaded(this->main_font,(const char*)text_data_in_out,this->selected_text_color,this->selected_text_background_color);
                        this->combine_surfaces(0,0,text0,this->main_surface);
                        SDL_FreeSurface(text0);
                        SDL_DestroyTexture(this->main_texture);
                        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                        int w_beam,h_beam;

                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                        this->rect_beam.x = this->base_beam_x+w_beam+1;

                        #ifdef TEXBOX_DEBUG
                        printf("    selected whole text\n\n");
                        #endif

                        this->beam_timer0 = SDL_GetTicks();
                        this->beam_animation_direction = 0;

                    }
                    else if(event->key.keysym.sym==SDLK_x&&((this->select_end_position_x-this->select_start_position_x)!=0))
                    {
                        if(this->select_start_position_x>this->select_end_position_x)
                        {
                        //from right to left

                            uint8_t* data_to_clipboard = (uint8_t*)malloc(this->select_start_position_x-this->select_end_position_x+1);
                            uint32_t i = 0;
                            uint32_t k = 0;
                            for(; ; i++,k++)
                            {
                                if(text_data_in_out[k]=='\0')
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    break;
                                }
                                else if(i==this->select_end_position_x)
                                {
                                    k = this->select_start_position_x;
                                    uint32_t l = 0;
                                    for(uint32_t j = this->select_end_position_x; j!=this->select_start_position_x; j++,l++) data_to_clipboard[l] = text_data_in_out[j];
                                    data_to_clipboard[l] = '\0';
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(text_data_in_out[k]=='\0') break;
                                }
                                else
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                }

                            }
                            #ifdef TEXBOX_DEBUG
                            printf("    actual_text=%s\n    text_in_clipboard=%s\n\n",text_data_in_out,data_to_clipboard);
                            #endif

                            if(SDL_SetClipboardText((const char*)data_to_clipboard)!=0)
                            {
                                #ifdef TEXBOX_DEBUG
                                fprintf(stderr,"failed copy to clipboard\n");
                                #endif
                            }

                            free(data_to_clipboard);

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            if(i!=0)
                            {
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);

                                int w_beam,h_beam;
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                this->width_of_text_texture = w_beam;
                            }
                            else this->width_of_text_texture = 0;

                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            this->how_many_chars_in_buffor = i;
                            this->select_start_position_x = this->select_end_position_x;

                        }
                        else
                        {
                        //from left to right

                            uint8_t* data_to_clipboard = (uint8_t*)malloc(this->select_end_position_x-this->select_start_position_x+1);
                            uint32_t i = 0;
                            uint32_t k = 0;
                            uint32_t goes_through = 0;
                            uint32_t new_beam_position = 0;
                            for(; ; i++,k++)
                            {
                                if(text_data_in_out[k]=='\0')
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    break;
                                }
                                else if(i==this->select_start_position_x)
                                {
                                    k = this->select_end_position_x;
                                    goes_through = 1;

                                    uint32_t l = 0;
                                    for(uint32_t j = this->select_start_position_x; j!=this->select_end_position_x; j++,l++) data_to_clipboard[l] = text_data_in_out[j];
                                    data_to_clipboard[l] = '\0';

                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(text_data_in_out[k]=='\0') break;
                                }
                                else
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(goes_through==0) new_beam_position++;
                                }

                            }

                            #ifdef TEXBOX_DEBUG
                            printf("    actual_text=%s\n    text_in_clipboard=%s\n\n",text_data_in_out,data_to_clipboard);
                            #endif

                            if(SDL_SetClipboardText((const char*)data_to_clipboard)!=0)
                            {
                                #ifdef TEXBOX_DEBUG
                                fprintf(stderr,"failed copy to clipboard\n");
                                #endif
                            }
                            free(data_to_clipboard);

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            if(i!=0)
                            {
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);

                                int w_beam,h_beam;
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                this->width_of_text_texture = w_beam;

                                uint8_t const_char_data[2] = {'\0','\0'};
                                const_char_data[0] = text_data_in_out[this->select_start_position_x];
                                text_data_in_out[this->select_start_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                this->rect_beam.x = this->base_beam_x+w_beam+1;
                                this->at_what_position_is_beam = new_beam_position;
                                text_data_in_out[this->select_start_position_x] = const_char_data[0];
                            }
                            else
                            {
                                this->width_of_text_texture = 0;

                                this->rect_beam.x = this->base_beam_x+1;
                                this->at_what_position_is_beam = 0;
                            }

                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            this->how_many_chars_in_buffor = i;
                            this->select_end_position_x = this->select_start_position_x;

                        }
                    }
                    else if(event->key.keysym.sym==SDLK_v)
                    {
                        char* clipboard_data = SDL_GetClipboardText();
                        if(clipboard_data[0]=='\0')
                        {
                            #ifdef TEXBOX_DEBUG
                            printf("    clipboard is empty or function failed\n\n",clipboard_data);
                            #endif
                            SDL_free(clipboard_data);
                            break;
                        }

                        uint32_t clipboard_data_counter = 0;
                        uint8_t is_char_allowed;
                        for(; clipboard_data[clipboard_data_counter]!='\0'; clipboard_data_counter++)
                        {
                            is_char_allowed = 0;
                            switch(this->mode)
                            {
                                case MODE_LETTERS:
                                {
                                    if((clipboard_data[clipboard_data_counter]>=97&&clipboard_data[clipboard_data_counter]<=122)||(clipboard_data[clipboard_data_counter]>=65&&clipboard_data[clipboard_data_counter]<=90)
                                       ||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode||clipboard_data[clipboard_data_counter]==SDLK_SPACE))
                                    {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                    }

                                }break;

                                case MODE_LETTERS_WITH_SYMBOLS:
                                {
                                    if((clipboard_data[clipboard_data_counter]>=58&&clipboard_data[clipboard_data_counter]<=126)||(clipboard_data[clipboard_data_counter]>=32&&clipboard_data[clipboard_data_counter]<=47)
                                        ||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode))
                                   {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                   }

                                }break;

                                case MODE_NUMBERS:
                                {
                                    if((clipboard_data[clipboard_data_counter]>=48&&clipboard_data[clipboard_data_counter]<=57)
                                       ||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode||clipboard_data[clipboard_data_counter]==SDLK_SPACE))
                                    {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                    }
                                }break;

                                case MODE_NUMBERS_WITH_SYMBOLS:
                                {
                                       if((clipboard_data[clipboard_data_counter]>=32&&clipboard_data[clipboard_data_counter]<=96)
                                          ||(clipboard_data[clipboard_data_counter]>=123&&clipboard_data[clipboard_data_counter]<=126)||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode))
                                       {
                                            if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                            else is_char_allowed = 1;
                                       }
                                }break;

                                case MODE_ALL:
                                {
                                    if(((clipboard_data[clipboard_data_counter]>=32&&clipboard_data[clipboard_data_counter]<=126))||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode))
                                    {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                    }
                                }break;

                                case MODE_SMALL_LETTERS:
                                {
                                    if((clipboard_data[clipboard_data_counter]>=97&&clipboard_data[clipboard_data_counter]<=122)
                                       ||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode||clipboard_data[clipboard_data_counter]==SDLK_SPACE))
                                    {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                    }
                                }break;

                                case MODE_SMALL_LETTERS_WITH_NUMBERS:
                                {

                                    if((clipboard_data[clipboard_data_counter]>=97&&clipboard_data[clipboard_data_counter]<=122)||((event->key.keysym.sym>=48&&event->key.keysym.sym<=57))
                                       ||(clipboard_data[clipboard_data_counter]==this->extra_char_allowed_to_mode||clipboard_data[clipboard_data_counter]==SDLK_SPACE))
                                    {
                                        if(clipboard_data[clipboard_data_counter]==SDLK_SPACE&&this->is_space_allowed==0) break;
                                        else is_char_allowed = 1;
                                    }
                                }break;

                                default: break;

                            }
                            if(is_char_allowed==0) break;
                        }
                        if(clipboard_data[clipboard_data_counter]!='\0')
                        {
                            #ifdef TEXBOX_DEBUG
                            printf("    paste failed 'cause in clipboard aren't allowed char/s\n\n");
                            #endif
                            SDL_free(clipboard_data);
                            break;
                        }
                        uint32_t total_chars = clipboard_data_counter+this->how_many_chars_in_buffor-SDL_abs(this->select_end_position_x-this->select_start_position_x);
                        if(total_chars>this->max_chars)
                        {
                            #ifdef TEXBOX_DEBUG
                            printf("    limit of max chars reached\n\n");
                            #endif
                            SDL_free(clipboard_data);
                            break;
                        }

                        if(this->select_end_position_x!=this->select_start_position_x)
                        {
                            if(this->select_start_position_x>this->select_end_position_x)
                            {
                            //from right to left

                                uint32_t visual_length_of_first_part;
                                uint32_t visual_length_of_second_part;

                                int w_beam,h_beam;

                            //**      clipboard
                            //__!!!__ text

                                uint8_t* buffor = (uint8_t*)malloc((total_chars-this->select_end_position_x)+1);
                                int32_t i = 0;
                                for(; i!=clipboard_data_counter; i++) buffor[i] = clipboard_data[i];
                                for(uint32_t j = this->select_start_position_x; text_data_in_out[j]!='\0'; i++,j++)
                                {
                                    buffor[i] = text_data_in_out[j];
                                }
                                buffor[i] = '\0';

                            //**      clipboard
                            //__!!!__ text
                            //**__    buffor

                                TTF_SizeUTF8(this->main_font,(const char*)buffor,&w_beam,&h_beam);
                                visual_length_of_second_part = w_beam;

                                uint8_t const_char_data[2] = {'\0','\0'};
                                const_char_data[0] = text_data_in_out[this->select_end_position_x];
                                text_data_in_out[this->select_end_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                visual_length_of_first_part = w_beam;
                                text_data_in_out[this->select_end_position_x] = const_char_data[0];

                                uint32_t j = this->select_end_position_x;
                                for(uint32_t k = 0; buffor[k]!='\0'; j++,k++) text_data_in_out[j] = buffor[k];
                                text_data_in_out[j] = '\0';

                            //**      clipboard
                            //__**__ text
                            //**__    buffor

                                TTF_SizeUTF8(this->main_font,(const char*)clipboard_data,&w_beam,&h_beam);


                                this->rect_beam.x = this->base_beam_x+visual_length_of_first_part+w_beam+1;
                                this->width_of_text_texture = this->base_beam_x+visual_length_of_first_part+visual_length_of_second_part+1;
                                this->how_many_chars_in_buffor = total_chars;
                                this->at_what_position_is_beam+=clipboard_data_counter;
                                this->select_end_position_x+=clipboard_data_counter;
                                this->select_start_position_x = select_end_position_x;


                                free(buffor);


                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                                /*printf("how_many_chars_in_buffor=%d\nat_what_position_is_beam=%d\nselect_end_position_x=%d\ntotal_chars=%d\n",
                                       this->how_many_chars_in_buffor,this->at_what_position_is_beam,this->select_end_position_x,
                                       total_chars);*/

                            }
                            else
                            {
                            //from left to right

                                uint32_t visual_length_of_first_part;
                                uint32_t visual_length_of_second_part;

                                int w_beam,h_beam;

                            //**      clipboard
                            //__!!!__ text

                                uint8_t* buffor = (uint8_t*)malloc((total_chars-this->select_start_position_x)+1);
                                int32_t i = 0;
                                for(; i!=clipboard_data_counter; i++) buffor[i] = clipboard_data[i];
                                for(uint32_t j = this->select_end_position_x; text_data_in_out[j]!='\0'; i++,j++)
                                {
                                    buffor[i] = text_data_in_out[j];
                                }
                                buffor[i] = '\0';

                            //**      clipboard
                            //__!!!__ text
                            //**__    buffor

                                TTF_SizeUTF8(this->main_font,(const char*)buffor,&w_beam,&h_beam);
                                visual_length_of_second_part = w_beam;

                                uint8_t const_char_data[2] = {'\0','\0'};
                                const_char_data[0] = text_data_in_out[this->select_start_position_x];
                                text_data_in_out[this->select_start_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                visual_length_of_first_part = w_beam;
                                text_data_in_out[this->select_start_position_x] = const_char_data[0];

                                uint32_t j = this->select_start_position_x;
                                for(uint32_t k = 0; buffor[k]!='\0'; j++,k++) text_data_in_out[j] = buffor[k];
                                text_data_in_out[j] = '\0';

                            //**      clipboard
                            //__**__ text
                            //**__    buffor

                                TTF_SizeUTF8(this->main_font,(const char*)clipboard_data,&w_beam,&h_beam);

                                this->rect_beam.x = this->base_beam_x+visual_length_of_first_part+w_beam+1;
                                this->width_of_text_texture = this->base_beam_x+visual_length_of_first_part+visual_length_of_second_part+1;
                                this->how_many_chars_in_buffor = total_chars;
                                this->at_what_position_is_beam = this->select_start_position_x+clipboard_data_counter;
                                this->select_end_position_x = this->at_what_position_is_beam;
                                this->select_start_position_x = select_end_position_x;

                                free(buffor);

                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                                /*printf("how_many_chars_in_buffor=%d\nat_what_position_is_beam=%d\nselect_end_position_x=%d\ntotal_chars=%d\n",
                                this->how_many_chars_in_buffor,this->at_what_position_is_beam,this->select_end_position_x,
                                total_chars);*/





                            }
                        }
                        else
                        {
                            int w_beam,h_beam;

                            uint8_t* buffor = (uint8_t*)malloc((total_chars-this->select_start_position_x)+1);

                            uint32_t i = 0;
                            for(; i!=clipboard_data_counter; i++) buffor[i] = clipboard_data[i];
                            for(uint32_t j = this->select_start_position_x; i!=(total_chars-this->select_start_position_x); i++,j++)
                            {
                                buffor[i] = text_data_in_out[j];
                            }
                            buffor[i] = '\0';
                            uint32_t j = this->select_start_position_x;
                            for(uint32_t k = 0; j!=total_chars; j++,k++) text_data_in_out[j] = buffor[k];
                            text_data_in_out[j] = '\0';

                            free(buffor);

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            SDL_Surface* text0;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            this->combine_surfaces(0,0,text0,this->main_surface);
                            SDL_FreeSurface(text0);
                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                            this->how_many_chars_in_buffor+=clipboard_data_counter;
                            this->at_what_position_is_beam+=clipboard_data_counter;
                            this->select_start_position_x+=clipboard_data_counter;
                            this->select_end_position_x = this->select_start_position_x;

                            uint8_t last_buffor = '\0';
                            last_buffor = text_data_in_out[this->select_start_position_x];
                            text_data_in_out[this->select_start_position_x] = '\0';

                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                            this->rect_beam.x = this->base_beam_x+w_beam+1;
                            text_data_in_out[this->select_start_position_x] = last_buffor;
                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                            this->width_of_text_texture = w_beam;

                        }
                        #ifdef TEXBOX_DEBUG
                        printf("    actual_text=%s\n",text_data_in_out);
                        printf("    pasted_text_from_clipboard=%s\n\n",clipboard_data);
                        #endif
                        SDL_free(clipboard_data);
                    }
                }
                else if(char0!='\0')
                {
                    if(this->select_end_position_x!=this->select_start_position_x)
                    {
                        if(this->select_start_position_x>this->select_end_position_x)
                        {
                        //from right to left

                            uint32_t i = 0;
                            uint32_t k = 0;
                            uint32_t new_beam_position = 1;
                            uint32_t goes_through = 0;
                            for(; ; i++,k++)
                            {
                                if(text_data_in_out[k]=='\0')
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    break;
                                }
                                else if(i==this->select_end_position_x)
                                {
                                    text_data_in_out[i] = char0;
                                    i++;
                                    goes_through = 1;
                                    k = this->select_start_position_x;
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(text_data_in_out[k]=='\0') break;
                                }
                                else
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(goes_through==0) new_beam_position++;
                                }

                            }

                            #ifdef TEXBOX_DEBUG
                            printf("    actual_text=%s\n\n",text_data_in_out);
                            #endif

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            SDL_Surface* text0;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            this->combine_surfaces(0,0,text0,this->main_surface);
                            SDL_FreeSurface(text0);
                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            this->how_many_chars_in_buffor = i;
                            this->select_end_position_x+=1;
                            this->select_start_position_x = this->select_end_position_x;


                            int w_beam,h_beam;

                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                            this->width_of_text_texture = w_beam;

                            uint8_t const_char_data[2] = {(uint8_t)char0,'\0'};
                            TTF_SizeUTF8(this->main_font,(const char*)const_char_data,&w_beam,&h_beam);
                            this->rect_beam.x+=w_beam;
                            this->at_what_position_is_beam = new_beam_position;


                        }
                        else
                        {
                        //from left to right

                            uint32_t i = 0;
                            uint32_t k = 0;
                            uint32_t goes_through = 0;
                            uint32_t new_beam_position = 1;
                            for(; ; i++,k++)
                            {
                                if(text_data_in_out[k]=='\0')
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    break;
                                }
                                else if(i==this->select_start_position_x)
                                {
                                    text_data_in_out[i] = char0;
                                    i++;
                                    goes_through = 1;
                                    k = this->select_end_position_x;
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(text_data_in_out[k]=='\0') break;
                                }
                                else
                                {
                                    text_data_in_out[i] = text_data_in_out[k];
                                    if(goes_through==0) new_beam_position++;
                                }
                            }

                            #ifdef TEXBOX_DEBUG
                            printf("    actual_text=%s\n\n",text_data_in_out);
                            #endif

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            SDL_Surface* text0;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            this->combine_surfaces(0,0,text0,this->main_surface);
                            SDL_FreeSurface(text0);
                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            this->how_many_chars_in_buffor = i;
                            this->select_start_position_x+=1;
                            this->select_end_position_x = this->select_start_position_x;

                            int w_beam,h_beam;

                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                            this->width_of_text_texture = w_beam;

                            uint8_t const_char_data[2] = {'\0','\0'};
                            const_char_data[0] = text_data_in_out[this->select_start_position_x];
                            text_data_in_out[this->select_start_position_x] = '\0';
                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                            this->rect_beam.x = this->base_beam_x+w_beam+1;
                            this->at_what_position_is_beam = new_beam_position;
                            text_data_in_out[this->select_start_position_x] = const_char_data[0];

                        }
                    }
                    else if(this->how_many_chars_in_buffor!=this->max_chars)
                    {
                        int old_text_w_beam,old_text_h_beam;
                        int new_text_w_beam,new_text_h_beam;
                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&old_text_w_beam,&old_text_h_beam);

                        uint32_t i;
                        for(i = this->how_many_chars_in_buffor+1; i!=this->at_what_position_is_beam; i--) text_data_in_out[i] = text_data_in_out[i-1];
                        text_data_in_out[i] = char0;

                        SDL_FreeSurface(this->main_surface);
                        this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                        SDL_Surface* text0;
                        if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                        this->combine_surfaces(0,0,text0,this->main_surface);
                        SDL_FreeSurface(text0);
                        SDL_DestroyTexture(this->main_texture);
                        this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                        this->how_many_chars_in_buffor++;
                        this->select_end_position_x++;
                        this->select_start_position_x++;

                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&new_text_w_beam,&new_text_h_beam);

                        this->width_of_text_texture+=(new_text_w_beam-old_text_w_beam);
                        this->at_what_position_is_beam++;
                        this->rect_beam.x+=(new_text_w_beam-old_text_w_beam);

                        #ifdef TEXBOX_DEBUG
                        printf("    actual_text=%s\n\n",text_data_in_out);
                        #endif
                    }
                    else
                    {
                            #ifdef TEXBOX_DEBUG
                            printf("    limit of max chars reached\n\n");
                            #endif
                            this->beam_timer0 = SDL_GetTicks();
                            this->beam_animation_direction = 0;
                            break;
                    }

                    this->beam_timer0 = SDL_GetTicks();
                    this->beam_animation_direction = 0;
                }
                else
                {
                    if(event->key.keysym.sym==SDLK_LEFT||event->key.keysym.sym==SDLK_DOWN)
                    {
                        if(this->at_what_position_is_beam!=0)
                        {
                            if(key_mode==KMOD_LSHIFT||key_mode==KMOD_RSHIFT)
                            {
                                int w_beam,h_beam;
                                uint32_t until_what_position_read;
                                uint8_t is_start_is_bigger_than_end = (this->select_start_position_x>=this->select_end_position_x) ? 1 : 0;
                                this->select_end_position_x--;
                                this->at_what_position_is_beam--;

                            //setting beam position
                                uint8_t const_char_data = '\0';
                                const_char_data = text_data_in_out[this->select_end_position_x];
                                text_data_in_out[this->select_end_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                text_data_in_out[this->select_end_position_x] = const_char_data;
                                this->rect_beam.x = this->base_beam_x+w_beam+1;

                            //first
                                SDL_Surface* first_part = NULL;
                                uint32_t first_part_width = 0;
                                if(is_start_is_bigger_than_end==1)  until_what_position_read = this->select_end_position_x;
                                else until_what_position_read = this->select_start_position_x;
                                if(this->select_end_position_x!=0)
                                {
                                    uint8_t* first_part_data = (uint8_t*)malloc(until_what_position_read+1);
                                    uint32_t i = 0;
                                    for(; i!=until_what_position_read ; i++) first_part_data[i] = text_data_in_out[i];
                                    first_part_data[i] = '\0';
                                    TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                                    first_part_width = w_beam;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                                    else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                                    free(first_part_data);
                                }

                            //second
                                SDL_Surface* second_part = NULL;
                                uint32_t second_part_width = 0;
                                if(is_start_is_bigger_than_end==1) until_what_position_read = this->select_start_position_x;
                                else until_what_position_read = this->select_end_position_x;
                                if(this->select_end_position_x!=this->select_start_position_x)
                                {
                                    uint8_t* second_part_data = (uint8_t*)malloc(((is_start_is_bigger_than_end) ? this->select_start_position_x-this->select_end_position_x : this->select_end_position_x-this->select_start_position_x)+1);
                                    for(uint32_t i = (is_start_is_bigger_than_end) ? this->select_end_position_x : this->select_start_position_x,k = 0; ; i++,k++)
                                    {
                                        if(i==until_what_position_read)
                                        {
                                            second_part_data[k] = '\0';
                                            break;
                                        }
                                        second_part_data[k] = text_data_in_out[i];
                                    }
                                    #ifdef TEXBOX_DEBUG
                                    printf("    select_start=%d\n    select_end=%d\n",this->select_start_position_x,this->select_end_position_x);
                                    printf("        text=%s\n\n",second_part_data);
                                    #endif
                                    TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                                    second_part_width = w_beam;
                                    if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                                    else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                                    free(second_part_data);
                                }


                            //third
                                uint32_t third_part_width = 0;
                                SDL_Surface* third_part = NULL;
                                if(this->select_start_position_x!=this->how_many_chars_in_buffor)
                                {
                                    uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-until_what_position_read+1);
                                    for(uint32_t i = until_what_position_read,k = 0; ; i++,k++)
                                    {
                                        if(i==this->how_many_chars_in_buffor)
                                        {
                                            third_part_data[k] = '\0';
                                            break;
                                        }
                                        third_part_data[k] = text_data_in_out[i];
                                    }
                                    TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                                    third_part_width = w_beam;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                                    else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                                    free(third_part_data);
                                }

                            //combine
                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                if(first_part!=NULL)
                                {
                                    this->combine_surfaces(0,0,first_part,this->main_surface);
                                    SDL_FreeSurface(first_part);
                                    if(second_part!=NULL)
                                    {
                                        this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                                        SDL_FreeSurface(second_part);
                                        if(third_part!=NULL)
                                        {
                                            this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                                            SDL_FreeSurface(third_part);
                                        }
                                    }
                                    else if(third_part!=NULL)
                                    {
                                        this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                                        SDL_FreeSurface(third_part);
                                    }


                                }
                                else if(second_part!=NULL)
                                {
                                    this->combine_surfaces(0,0,second_part,this->main_surface);
                                    SDL_FreeSurface(second_part);
                                    if(third_part!=NULL)
                                    {
                                        this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                                        SDL_FreeSurface(third_part);
                                    }

                                }
                                else
                                {
                                    this->combine_surfaces(0,0,third_part,this->main_surface);
                                    SDL_FreeSurface(third_part);

                                }
                                this->width_of_text_texture = first_part_width+second_part_width+third_part_width;
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            }
                            else
                            {

                                if(this->select_end_position_x!=this->select_start_position_x)
                                {
                                    if(this->select_start_position_x<this->select_end_position_x)
                                    {
                                        int w_beam,h_beam;
                                        uint8_t const_char_data[2] = {'\0','\0'};
                                        const_char_data[0] = text_data_in_out[this->select_start_position_x];
                                        text_data_in_out[this->select_start_position_x] = '\0';
                                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                        this->rect_beam.x = this->base_beam_x+w_beam+1;
                                        this->at_what_position_is_beam = select_start_position_x;
                                        text_data_in_out[this->select_start_position_x] = const_char_data[0];
                                        this->select_end_position_x = this->select_start_position_x;
                                        #ifdef TEXBOX_DEBUG
                                        printf("    beam_position=%d\n\n",this->at_what_position_is_beam);
                                        #endif
                                    }
                                    else
                                    {
                                        this->select_end_position_x = this->at_what_position_is_beam;
                                        this->select_start_position_x = this->at_what_position_is_beam;
                                    }


                                    SDL_FreeSurface(this->main_surface);
                                    this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                    SDL_Surface* text0;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    this->combine_surfaces(0,0,text0,this->main_surface);
                                    SDL_FreeSurface(text0);
                                    SDL_DestroyTexture(this->main_texture);
                                    this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                                }
                                else
                                {
                                    uint8_t buffor = text_data_in_out[this->at_what_position_is_beam-1];
                                    text_data_in_out[this->at_what_position_is_beam-1] = '\0';
                                    int w_beam,h_beam;
                                    TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                    text_data_in_out[this->at_what_position_is_beam-1] = buffor;
                                    this->at_what_position_is_beam--;
                                    this->select_end_position_x--;
                                    this->select_start_position_x--;
                                    this->rect_beam.x = this->base_beam_x+w_beam+1;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    beam_position=%d\n\n",this->at_what_position_is_beam);
                                    #endif
                                }
                            }
                        }
                        else
                        {
                            if((this->select_end_position_x-this->select_start_position_x)!=0&&(key_mode!=KMOD_LSHIFT&&key_mode!=KMOD_RSHIFT))
                            {
                                this->select_end_position_x = this->at_what_position_is_beam;
                                this->select_start_position_x = this->at_what_position_is_beam;

                                int w_beam,h_beam;
                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            }
                        }
                        this->beam_timer0 = SDL_GetTicks();
                        this->beam_animation_direction = 0;
                    }
                    else if(event->key.keysym.sym==SDLK_RIGHT||event->key.keysym.sym==SDLK_UP)
                    {
                        if(this->at_what_position_is_beam!=this->how_many_chars_in_buffor)
                        {
                            if(key_mode==KMOD_LSHIFT||key_mode==KMOD_RSHIFT)
                            {
                                int w_beam,h_beam;
                                uint32_t until_what_position_read;
                                uint8_t is_start_is_bigger_than_end = (this->select_start_position_x>this->select_end_position_x) ? 1 : 0;
                                this->select_end_position_x++;
                                this->at_what_position_is_beam++;

                            //setting beam position
                                uint8_t const_char_data = '\0';
                                const_char_data = text_data_in_out[this->select_end_position_x];
                                text_data_in_out[this->select_end_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                text_data_in_out[this->select_end_position_x] = const_char_data;
                                this->rect_beam.x = this->base_beam_x+w_beam+1;

                            //first
                                SDL_Surface* first_part = NULL;
                                uint32_t first_part_width = 0;
                                if(is_start_is_bigger_than_end==1)  until_what_position_read = this->select_end_position_x;
                                else until_what_position_read = this->select_start_position_x;
                                if(this->select_start_position_x!=0)
                                {
                                    uint8_t* first_part_data = (uint8_t*)malloc(until_what_position_read+1);
                                    uint32_t i = 0;
                                    for(; i!=until_what_position_read ; i++) first_part_data[i] = text_data_in_out[i];
                                    first_part_data[i] = '\0';
                                    TTF_SizeUTF8(this->main_font,(const char*)first_part_data,&w_beam,&h_beam);
                                    first_part_width = w_beam;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) first_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)first_part_data,this->solid_text_color);
                                    else first_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)first_part_data,this->solid_text_color);
                                    free(first_part_data);
                                }

                            //second
                                SDL_Surface* second_part = NULL;
                                uint32_t second_part_width = 0;
                                if(is_start_is_bigger_than_end==1) until_what_position_read = this->select_start_position_x;
                                else until_what_position_read = this->select_end_position_x;
                                if(this->select_end_position_x!=this->select_start_position_x)
                                {
                                    uint8_t* second_part_data = (uint8_t*)malloc(((is_start_is_bigger_than_end) ? this->select_start_position_x-this->select_end_position_x : this->select_end_position_x-this->select_start_position_x)+1);
                                    for(uint32_t i = (is_start_is_bigger_than_end) ? this->select_end_position_x : this->select_start_position_x,k = 0; ; i++,k++)
                                    {
                                        if(i==until_what_position_read)
                                        {
                                            second_part_data[k] = '\0';
                                            break;
                                        }
                                        second_part_data[k] = text_data_in_out[i];
                                    }
                                    #ifdef TEXBOX_DEBUG
                                    printf("    select_start=%d\n    select_end=%d\n",this->select_start_position_x,this->select_end_position_x);
                                    printf("        text=%s\n\n",second_part_data);
                                    #endif
                                    TTF_SizeUTF8(this->main_font,(const char*)second_part_data,&w_beam,&h_beam);
                                    second_part_width = w_beam;
                                    if(this->selected_font_quality==TextBoxClass::SELECTED_FONT_QUALITY_LCD) second_part = TTF_RenderUTF8_LCD(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                                    else second_part = TTF_RenderUTF8_Shaded(this->main_font,(const char*)second_part_data,this->selected_text_color,this->selected_text_background_color);
                                    free(second_part_data);
                                }

                            //third
                                uint32_t third_part_width = 0;
                                SDL_Surface* third_part = NULL;
                                if(this->select_end_position_x!=this->how_many_chars_in_buffor)
                                {
                                    uint8_t* third_part_data = (uint8_t*)malloc(this->how_many_chars_in_buffor-until_what_position_read+1);
                                    for(uint32_t i = until_what_position_read,k = 0; ; i++,k++)
                                    {
                                        if(i==this->how_many_chars_in_buffor)
                                        {
                                            third_part_data[k] = '\0';
                                            break;
                                        }
                                        third_part_data[k] = text_data_in_out[i];
                                    }
                                    TTF_SizeUTF8(this->main_font,(const char*)third_part_data,&w_beam,&h_beam);
                                    third_part_width = w_beam;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) third_part = TTF_RenderUTF8_Solid(this->main_font,(const char*)third_part_data,this->solid_text_color);
                                    else third_part = TTF_RenderUTF8_Blended(this->main_font,(const char*)third_part_data,this->solid_text_color);
                                    free(third_part_data);
                                }

                                //combine
                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                if(first_part!=NULL)
                                {
                                    this->combine_surfaces(0,0,first_part,this->main_surface);
                                    SDL_FreeSurface(first_part);
                                    if(second_part!=NULL)
                                    {
                                        this->combine_surfaces(first_part_width,0,second_part,this->main_surface);
                                        SDL_FreeSurface(second_part);
                                        if(third_part!=NULL)
                                        {
                                            this->combine_surfaces(first_part_width+second_part_width,0,third_part,this->main_surface);
                                            SDL_FreeSurface(third_part);
                                        }
                                    }
                                    else if(third_part!=NULL)
                                    {
                                        this->combine_surfaces(first_part_width,0,third_part,this->main_surface);
                                        SDL_FreeSurface(third_part);
                                    }


                                }
                                else if(second_part!=NULL)
                                {
                                    this->combine_surfaces(0,0,second_part,this->main_surface);
                                    SDL_FreeSurface(second_part);
                                    if(third_part!=NULL)
                                    {
                                        this->combine_surfaces(second_part_width,0,third_part,this->main_surface);
                                        SDL_FreeSurface(third_part);
                                    }

                                }
                                else
                                {
                                    this->combine_surfaces(0,0,third_part,this->main_surface);
                                    SDL_FreeSurface(third_part);

                                }
                                this->width_of_text_texture = first_part_width+second_part_width+third_part_width;
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                            }
                            else
                            {
                                if(this->select_end_position_x!=this->select_start_position_x)
                                {
                                    if(this->select_start_position_x>this->select_end_position_x)
                                    {
                                        int w_beam,h_beam;
                                        uint8_t const_char_data[2] = {'\0','\0'};
                                        const_char_data[0] = text_data_in_out[this->select_start_position_x];
                                        text_data_in_out[this->select_start_position_x] = '\0';
                                        TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                        this->rect_beam.x = this->base_beam_x+w_beam+1;
                                        this->at_what_position_is_beam = select_start_position_x;
                                        text_data_in_out[this->select_start_position_x] = const_char_data[0];
                                        this->select_end_position_x = this->select_start_position_x;
                                        #ifdef TEXBOX_DEBUG
                                        printf("    beam_position=%d\n\n",this->at_what_position_is_beam);
                                        #endif

                                    }
                                    else
                                    {
                                        this->select_end_position_x = this->at_what_position_is_beam;
                                        this->select_start_position_x = this->at_what_position_is_beam;
                                    }

                                    SDL_FreeSurface(this->main_surface);
                                    this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                    SDL_Surface* text0;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    this->combine_surfaces(0,0,text0,this->main_surface);
                                    SDL_FreeSurface(text0);
                                    SDL_DestroyTexture(this->main_texture);
                                    this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);

                                }
                                else
                                {
                                    uint8_t buffor = text_data_in_out[this->at_what_position_is_beam+1];
                                    text_data_in_out[this->at_what_position_is_beam+1] = '\0';
                                    int w_beam,h_beam;
                                    TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                    text_data_in_out[this->at_what_position_is_beam+1] = buffor;
                                    this->at_what_position_is_beam++;
                                    this->select_end_position_x++;
                                    this->select_start_position_x++;
                                    this->rect_beam.x = this->base_beam_x+w_beam+1;
                                    #ifdef TEXBOX_DEBUG
                                    printf("    beam_position=%d\n\n",this->at_what_position_is_beam);
                                    #endif
                                }

                            }
                        }
                        else
                        {
                            if(((this->select_start_position_x-this->select_end_position_x)!=0)&&(key_mode!=KMOD_LSHIFT&&key_mode!=KMOD_RSHIFT))
                            {
                                this->select_end_position_x = this->at_what_position_is_beam;
                                this->select_start_position_x = this->at_what_position_is_beam;

                                int w_beam,h_beam;
                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                SDL_Surface* text0;
                                if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                this->combine_surfaces(0,0,text0,this->main_surface);
                                SDL_FreeSurface(text0);
                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            }
                        }

                        this->beam_timer0 = SDL_GetTicks();
                        this->beam_animation_direction = 0;
                    }
                    else if(event->key.keysym.sym==SDLK_BACKSPACE)
                    {
                        if(this->select_end_position_x!=this->select_start_position_x)
                        {
                            if(this->select_start_position_x>this->select_end_position_x)
                            {
                            //from right to left

                                uint32_t i = 0;
                                uint32_t k = 0;
                                for(; ; i++,k++)
                                {
                                    if(text_data_in_out[k]=='\0')
                                    {
                                        text_data_in_out[i] = text_data_in_out[k];
                                        break;
                                    }
                                    else if(i==this->select_end_position_x)
                                    {
                                        k = this->select_start_position_x;
                                        text_data_in_out[i] = text_data_in_out[k];
                                        if(text_data_in_out[k]=='\0') break;
                                    }
                                    else
                                    {
                                        text_data_in_out[i] = text_data_in_out[k];
                                    }

                                }
                                #ifdef TEXBOX_DEBUG
                                printf("    actual_text=%s\n\n",text_data_in_out);
                                #endif

                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                if(i!=0)
                                {
                                    SDL_Surface* text0;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    this->combine_surfaces(0,0,text0,this->main_surface);
                                    SDL_FreeSurface(text0);

                                    int w_beam,h_beam;
                                    TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                    this->width_of_text_texture = w_beam;
                                }
                                else this->width_of_text_texture = 0;

                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                                this->how_many_chars_in_buffor = i;
                                this->select_start_position_x = this->select_end_position_x;
                            }
                            else
                            {
                            //from left to right

                                uint32_t i = 0;
                                uint32_t k = 0;
                                for(; ; i++,k++)
                                {
                                    if(text_data_in_out[k]=='\0')
                                    {
                                        text_data_in_out[i] = text_data_in_out[k];
                                        break;
                                    }
                                    else if(i==this->select_start_position_x)
                                    {
                                        k = this->select_end_position_x;
                                        text_data_in_out[i] = text_data_in_out[k];
                                        if(text_data_in_out[k]=='\0') break;
                                    }
                                    else
                                    {
                                        text_data_in_out[i] = text_data_in_out[k];
                                    }
                                }

                                #ifdef TEXBOX_DEBUG
                                printf("    actual_text=%s\n\n",text_data_in_out);
                                #endif

                                int w_beam,h_beam;
                                SDL_FreeSurface(this->main_surface);
                                this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                                if(i!=0)
                                {
                                    SDL_Surface* text0;
                                    if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                                    this->combine_surfaces(0,0,text0,this->main_surface);
                                    SDL_FreeSurface(text0);

                                    TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                    this->width_of_text_texture = w_beam;
                                }
                                else this->width_of_text_texture = 0;

                                SDL_DestroyTexture(this->main_texture);
                                this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                                this->how_many_chars_in_buffor = i;
                                this->select_end_position_x = this->select_start_position_x;

                                uint8_t const_char_data[2] = {'\0','\0'};
                                const_char_data[0] = text_data_in_out[this->select_start_position_x];
                                text_data_in_out[this->select_start_position_x] = '\0';
                                TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&w_beam,&h_beam);
                                this->rect_beam.x = this->base_beam_x+w_beam+1;
                                this->at_what_position_is_beam = this->select_start_position_x;
                                text_data_in_out[this->select_start_position_x] = const_char_data[0];


                            }

                        }
                        else if(this->at_what_position_is_beam!=0)
                        {
                            int old_text_w_beam,old_text_h_beam;
                            int new_text_w_beam,new_text_h_beam;
                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&old_text_w_beam,&old_text_h_beam);

                            uint32_t i;
                            uint8_t start_copying = 0;
                            uint8_t buffor = '\0';
                            for(i = 0; i!=this->how_many_chars_in_buffor; i++)
                            {
                                if(i==this->at_what_position_is_beam-1)
                                {
                                    start_copying = 1;
                                    buffor = text_data_in_out[i];
                                    text_data_in_out[i] = text_data_in_out[i+1];
                                }
                                else if(start_copying==1)
                                {
                                    text_data_in_out[i] = text_data_in_out[i+1];
                                }
                            }

                            SDL_FreeSurface(this->main_surface);
                            this->main_surface = SDL_CreateRGBSurface(0,this->w,this->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                            SDL_Surface* text0;
                            if(this->solid_font_quality==TextBoxClass::SOLID_FONT_QUALITY_STANDARD) text0 = TTF_RenderUTF8_Solid(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            else text0 = TTF_RenderUTF8_Blended(this->main_font,(const char*)text_data_in_out,this->solid_text_color);
                            this->combine_surfaces(0,0,text0,this->main_surface);
                            SDL_FreeSurface(text0);
                            SDL_DestroyTexture(this->main_texture);
                            this->main_texture = SDL_CreateTextureFromSurface(this->render,this->main_surface);
                            this->how_many_chars_in_buffor--;
                            this->select_end_position_x--;
                            this->select_start_position_x--;

                            TTF_SizeUTF8(this->main_font,(const char*)text_data_in_out,&new_text_w_beam,&new_text_h_beam);

                            this->width_of_text_texture-=(old_text_w_beam-new_text_w_beam);
                            this->at_what_position_is_beam--;
                            this->rect_beam.x-=(old_text_w_beam-new_text_w_beam);

                            #ifdef TEXBOX_DEBUG
                            printf("    actual_text=%s\n\n",text_data_in_out);
                            #endif
                        }

                        this->beam_timer0 = SDL_GetTicks();
                        this->beam_animation_direction = 0;
                    }
                }

            }break;

    }
    if(do_display_textures==1) SDL_RenderCopy(this->render,this->main_texture,NULL,&this->main_texture_rect);

    if(this->focus==1)
    {
        if(this->beam_delay_in_ms==0&&do_display_textures==1)
        {
            this->beam_animation_direction = 0;
            SDL_RenderCopy(this->render,this->beam,NULL,&this->rect_beam);
        }
        else
        {
            this->beam_timer1 = SDL_GetTicks();
            if(this->beam_timer1-this->beam_timer0>=this->beam_delay_in_ms)
            {
                if(this->beam_animation_direction==0) this->beam_animation_direction = 1;
                else this->beam_animation_direction = 0;
                this->beam_timer0 = SDL_GetTicks();
            }
            if(this->beam_animation_direction==0&&do_display_textures==1) SDL_RenderCopy(this->render,this->beam,NULL,&this->rect_beam);
        }

    }
    else this->beam_animation_direction = 1;
    return;
}

uint8_t TextBoxClass::do_data_should_be_safed() const
{
    return this->do_data_should_be_safed_variable;
}

SDL_Texture* TextBoxClass::get_text_texture()
{
    return this->main_texture;
}

SDL_Rect* TextBoxClass::get_rect_of_text_texture()
{
    return &this->main_texture_rect;
}

SDL_Texture* TextBoxClass::get_beam_texture()
{
    return this->beam;
}

SDL_Rect* TextBoxClass::get_rect_of_beam_texture()
{
     return &this->rect_beam;
}

bool TextBoxClass::should_cursor_be_arrow_or_beam() const
{
    return this->cursor_display;
}

bool TextBoxClass::should_beam_be_showed_or_not() const
{
    if(this->beam_animation_direction==0) return 1;
    else return 0;
}

void TextBoxClass::update_textbox()
{
    this->updated_textbox = 1;
    return;
}

void TextBoxClass::set_max_chars(uint32_t limit_of_chars)
{
    this->max_chars = limit_of_chars;
    return;
}

uint32_t TextBoxClass::get_width_of_text_texture()
{
    return this->width_of_text_texture;
}
