//use std::cell::RefCell;
//use std::rc::Rc;

use std::os::raw::c_void;

use wrui::Ui;
use wrui::wrui::*;

pub struct PlayerView {
    wrui: Ui,
    player_display: Widget,
    display_font: Font,
    title: String,
    current_time: f32,
    pub widget: Widget,
}


impl PlayerView {
    pub fn new(wrui: Ui) -> PlayerView {
        PlayerView {
            wrui,
            current_time: 0.0,
            player_display: wrui.create_widget(),
            display_font: wrui.create_font(),
            widget: wrui.create_widget(),
            title: String::new(),
        }
    }

    pub fn set_current_time(&mut self, current_time: f32) {
        self.current_time = current_time;
        self.player_display.update();
    }

    pub fn draw_display(&mut self, event: &PaintEvent) {
        let painter = self.wrui.create_painter();

        let test_color = Color { r: 30, g: 30, b: 30, a: 255 };

        painter.begin(&self.player_display);
        painter.set_font(&self.display_font);
        painter.fill_rect_color(event.rect(), test_color);
        painter.draw_text(10, 40, &self.title);
        painter.draw_text(220, 80, &format!("{}:{:02}",
            (self.current_time / 60.0) as i32,
            (self.current_time % 60.0) as i32));


        //painter.draw_line(0, 0, 120, 120);
        painter.end();
    }

    pub fn set_title(&mut self, title: &str) {
        self.title = title.to_owned();
        self.player_display.update();
    }

    ///
    /// Creates the Ui
    ///
    pub fn setup(&mut self) {

        self.display_font.set_family("Arial");
        self.display_font.set_point_size(20);

        //let display = self.wrui.create_widget();
        let buttons = self.wrui.create_widget();
        let prev_button = self.wrui.create_push_button();
        let stop_button = self.wrui.create_push_button();
        let play_button = self.wrui.create_push_button();
        let next_button = self.wrui.create_push_button();

        prev_button.set_text("Prev");
        stop_button.set_text("Stop");
        play_button.set_text("Play");
        next_button.set_text("Next");

        self.player_display.set_fixed_height(100);
        self.player_display.set_fixed_width(500);

        set_paint_event!(self.player_display, self, PlayerView, PlayerView::draw_display);

        let vbox = self.wrui.create_v_box_layout();
        let hbox = self.wrui.create_h_box_layout();

        hbox.add_widget(&prev_button);
        hbox.add_widget(&stop_button);
        hbox.add_widget(&play_button);
        hbox.add_widget(&next_button);

        buttons.set_layout(&hbox);

        vbox.add_widget(&self.player_display);
        vbox.add_widget(&buttons);

        self.widget.set_layout(&vbox);
    }
}
