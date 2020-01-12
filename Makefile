# GCC/MinGW Makefile

CFLAGS = -O3 -Wall -I. -I./fltk -I./fltk/src -I./fltk/src/png -I./fltk/src/zlib -DNDEBUG -ffunction-sections -fdata-sections
CXXFLAGS = $(CFLAGS)
LDFLAGS = -Wl,--gc-sections -municode -mwindows -lcomctl32 -ldinput8 -ldxguid -lole32 -static

MINGW_PREFIX = i686-w64-mingw32-
MINGW_THREADS = -win32

CC = $(MINGW_PREFIX)gcc$(MINGW_THREADS)
CXX = $(MINGW_PREFIX)g++$(MINGW_THREADS)
AR = $(MINGW_PREFIX)ar
RANLIB = $(MINGW_PREFIX)ranlib
STRIP = $(MINGW_PREFIX)strip
WINDRES = $(MINGW_PREFIX)windres
XXD = xxd

BIN = SonicLauncher.exe
BIN_SRCFILES = configuration.cpp main.cpp
BIN_SRCS = $(addprefix $(CURDIR)/src/, $(BIN_SRCFILES))
BIN_OBJS = $(addsuffix .o, $(BIN_SRCS))

FLTK = libfltk.a
FLTK_SRCFILES = Fl.cxx Fl_Bitmap.cxx Fl_Browser_load.cxx Fl_Box.cxx Fl_Button.cxx Fl_Check_Button.cxx Fl_Choice.cxx Fl_Device.cxx Fl_Double_Window.cxx \
 Fl_Group.cxx Fl_Image.cxx Fl_Input.cxx Fl_Input_.cxx Fl_Light_Button.cxx Fl_Menu.cxx Fl_Menu_.cxx Fl_Menu_Button.cxx Fl_Menu_Window.cxx Fl_Menu_add.cxx \
 Fl_Overlay_Window.cxx Fl_Pixmap.cxx Fl_Preferences.cxx Fl_Repeat_Button.cxx Fl_Return_Button.cxx Fl_Shared_Image.cxx Fl_Single_Window.cxx Fl_Tabs.cxx \
 Fl_Tiled_Image.cxx Fl_Tooltip.cxx Fl_Widget.cxx Fl_Window.cxx Fl_Window_fullscreen.cxx Fl_Window_hotspot.cxx Fl_Window_iconize.cxx Fl_Window_shape.cxx \
 Fl_XBM_Image.cxx Fl_XPM_Image.cxx Fl_abort.cxx Fl_compose.cxx Fl_get_system_colors.cxx Fl_grab.cxx Fl_lock.cxx filename_list.cxx filename_match.cxx fl_arc.cxx \
 fl_arci.cxx fl_ask.cxx fl_boxtype.cxx fl_color.cxx fl_cursor.cxx fl_curve.cxx fl_dnd.cxx fl_draw.cxx fl_draw_image.cxx fl_draw_pixmap.cxx fl_font.cxx \
 fl_gleam.cxx fl_gtk.cxx fl_labeltype.cxx fl_line_style.cxx fl_overlay.cxx fl_overlay_visual.cxx fl_plastic.cxx fl_read_image.cxx fl_rect.cxx fl_round_box.cxx \
 fl_shortcut.cxx fl_symbols.cxx fl_vertex.cxx screen_xywh.cxx fl_utf8.cxx flstring.c scandir.c numericsort.c vsnprintf.c xutf8/is_right2left.c xutf8/is_spacing.c \
 xutf8/case.c xutf8/utf8Input.c xutf8/utf8Utils.c xutf8/utf8Wrap.c xutf8/keysym2Ucs.c fl_utf.c fl_call_main.c
# Fl_Adjuster.cxx Fl_Browser.cxx Fl_Browser_.cxx Fl_Chart.cxx Fl_Check_Browser.cxx Fl_Clock.cxx Fl_Color_Chooser.cxx Fl_Copy_Surface.cxx Fl_Counter.cxx
# Fl_Dial.cxx Fl_Help_Dialog_Dox.cxx Fl_File_Browser.cxx Fl_File_Chooser.cxx Fl_File_Chooser2.cxx Fl_File_Icon.cxx Fl_File_Input.cxx Fl_Help_View.cxx
# Fl_Image_Surface.cxx Fl_Menu_Bar.cxx Fl_Menu_global.cxx Fl_Multi_Label.cxx Fl_Native_File_Chooser.cxx Fl_Pack.cxx Fl_Paged_Device.cxx Fl_Positioner.cxx
# Fl_PostScript.cxx Fl_Printer.cxx Fl_Progress.cxx Fl_Roller.cxx Fl_Round_Button.cxx Fl_Scroll.cxx Fl_Scrollbar.cxx Fl_Slider.cxx Fl_Table.cxx Fl_Table_Row.cxx
# Fl_Text_Buffer.cxx Fl_Text_Display.cxx Fl_Text_Editor.cxx Fl_Tile.cxx Fl_Tree.cxx Fl_Tree_Item_Array.cxx Fl_Tree_Item.cxx Fl_Tree_Prefs.cxx Fl_Valuator.cxx
# Fl_Value_Input.cxx Fl_Value_Output.cxx Fl_Value_Slider.cxx Fl_Wizard.cxx Fl_add_idle.cxx Fl_arg.cxx Fl_display.cxx Fl_get_key.cxx Fl_own_colormap.cxx
# Fl_visual.cxx Fl_x.cxx filename_absolute.cxx filename_expand.cxx filename_ext.cxx filename_isdir.cxx filename_setext.cxx fl_diamond_box.cxx
# fl_engraved_label.cxx fl_file_dir.cxx fl_open_uri.cxx fl_oval_box.cxx fl_rounded_box.cxx fl_set_font.cxx fl_set_fonts.cxx fl_scroll_area.cxx fl_shadow_box.cxx
# fl_show_colormap.cxx ps_image.cxx fl_encoding_latin1.cxx fl_encoding_mac_roman.cxx
FLTK_SRCS = $(addprefix $(CURDIR)/fltk/src/src/, $(FLTK_SRCFILES))
FLTK_OBJS = $(addsuffix .o, $(FLTK_SRCS))

FLTK_IMAGES = libfltk_images.a
FLTK_IMAGES_SRCFILES = Fl_PNG_Image.cxx
#fl_images_core.cxx Fl_BMP_Image.cxx Fl_File_Icon2.cxx Fl_GIF_Image.cxx Fl_Help_Dialog.cxx Fl_JPEG_Image.cxx Fl_PNM_Image.cxx
FLTK_IMAGES_SRCS = $(addprefix $(CURDIR)/fltk/src/src/, $(FLTK_IMAGES_SRCFILES))
FLTK_IMAGES_OBJS = $(addsuffix .o, $(FLTK_IMAGES_SRCS))

FLTK_PNG = libpng.a
FLTK_PNG_SRCFILES = png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c
FLTK_PNG_SRCS = $(addprefix $(CURDIR)/fltk/src/png/, $(FLTK_PNG_SRCFILES))
FLTK_PNG_OBJS = $(addsuffix .o, $(FLTK_PNG_SRCS))

FLTK_ZLIB = libz.a
FLTK_ZLIB_SRCFILES = adler32.c compress.c crc32.c deflate.c inflate.c infback.c inftrees.c inffast.c trees.c uncompr.c zutil.c
FLTK_ZLIB_SRCS = $(addprefix $(CURDIR)/fltk/src/zlib/, $(FLTK_ZLIB_SRCFILES))
FLTK_ZLIB_OBJS = $(addsuffix .o, $(FLTK_ZLIB_SRCS))


all: $(BIN)

clean:
	rm -f $(BIN) SonicLauncher.rc.o images.h
	rm -f $(BIN_OBJS)

distclean: clean
	rm -f main.conf
	rm -f $(FLTK) $(FLTK_IMAGES) $(FLTK_PNG) $(FLTK_ZLIB)
	rm -f $(FLTK_OBJS)
	rm -f $(FLTK_IMAGES_OBJS)
	rm -f $(FLTK_PNG_OBJS)
	rm -f $(FLTK_ZLIB_OBJS)

IMAGES = arrow_01.png arrow_02.png arrow_03.png arrow_04.png back1.png back2.png back3.png \
 button_01.png button_02.png button_03.png button_04.png button_05.png icon.png pad_controls_v02.png

images.h:
	cd images; $(foreach img,$(IMAGES),$(XXD) -i $(img) >> ../images.h; )

$(BIN_OBJS): images.h
$(BIN_OBJS): CXXFLAGS+=-municode -mwindows

$(BIN): $(FLTK_ZLIB) $(FLTK_PNG) $(FLTK_IMAGES) $(FLTK) $(BIN_OBJS) SonicLauncher.rc.o
	$(CXX) -o $@ $(BIN_OBJS) SonicLauncher.rc.o $(FLTK_IMAGES) $(FLTK) $(FLTK_PNG) $(FLTK_ZLIB) $(LDFLAGS)
	$(STRIP) $@

$(FLTK): CXXFLAGS+=-DFL_LIBRARY
$(FLTK): $(FLTK_OBJS)
	$(AR) cru $@ $^ && $(RANLIB) $@

$(FLTK_IMAGES): CXXFLAGS+=-DFL_LIBRARY
$(FLTK_IMAGES): $(FLTK_IMAGES_OBJS)
	$(AR) cru $@ $^ && $(RANLIB) $@

$(FLTK_PNG): $(FLTK_PNG_OBJS)
	$(AR) cru $@ $^ && $(RANLIB) $@

$(FLTK_ZLIB): $(FLTK_ZLIB_OBJS)
	$(AR) cru $@ $^ && $(RANLIB) $@

%.rc.o:
	$(WINDRES) -i $(basename $@) -o $@

%.c.o:
	$(CC) $(CFLAGS) -c $(basename $@) -o $@

%.cxx.o:
	$(CXX) $(CXXFLAGS) -c $(basename $@) -o $@

%.cpp.o:
	$(CXX) $(CXXFLAGS) -c $(basename $@) -o $@

