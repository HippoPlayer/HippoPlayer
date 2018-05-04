//use std::cell::RefCell;
//use std::rc::Rc;

use std::os::raw::c_void;

use rute::Ui;
use rute::rute::*;

pub struct PlayerView {
    rute: Ui,
    player_display: Widget,
    display_font: Font,
    title: String,
    current_time: f32,
    pub prev_button: PushButton,
    pub stop_button: PushButton,
    pub play_button: PushButton,
    pub next_button: PushButton,
    pub widget: Widget,
}


impl PlayerView {
    pub fn new(rute: Ui) -> PlayerView {
        PlayerView {
            rute,
            current_time: -10.0,
            player_display: rute.create_widget(),
            display_font: rute.create_font(),
            prev_button: rute.create_push_button(),
            stop_button: rute.create_push_button(),
            play_button: rute.create_push_button(),
            next_button: rute.create_push_button(),
            widget: rute.create_widget(),
            title: String::new(),
        }
    }

    pub fn set_current_time(&mut self, current_time: f32) {
        self.current_time = current_time;
        self.player_display.update();
    }

    pub fn draw_display(&mut self, event: &PaintEvent) {
        let painter = self.rute.create_painter();

        let test_color = Color { r: 30, g: 30, b: 30, a: 255 };

        painter.begin(&self.player_display);
        painter.set_font(&self.display_font);
        painter.fill_rect_color(event.rect(), test_color);
        painter.draw_text(10, 40, &self.title);
        if self.current_time >= 0.0 {
            painter.draw_text(220, 80, &format!("{}:{:02}",
                (self.current_time / 60.0) as i32,
                (self.current_time % 60.0) as i32));
        } else {
            painter.draw_text(220, 80, "--:--");
        }

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

        //let display = self.rute.create_widget();
        let buttons = self.rute.create_widget();

        let prev_icon = self.rute.create_icon();
        let stop_icon = self.rute.create_icon();
        let play_icon = self.rute.create_icon();
        let next_icon = self.rute.create_icon();

        prev_icon.add_file("bin/player/buttons/hip_button_back.png");
        stop_icon.add_file("bin/player/buttons/hip_button_stop.png");
        play_icon.add_file("bin/player/buttons/hip_button_play.png");
        next_icon.add_file("bin/player/buttons/hip_button_next.png");

        //self.prev_button.set_text("Prev");
        //self.stop_button.set_text("Stop");
        //self.play_button.set_text("Play");
        //self.next_button.set_text("Next");

        self.prev_button.set_icon(&prev_icon);
        self.stop_button.set_icon(&stop_icon);
        self.play_button.set_icon(&play_icon);
        self.next_button.set_icon(&next_icon);

        self.player_display.set_fixed_height(100);
        self.player_display.set_fixed_width(500);

        set_paint_event!(self.player_display, self, PlayerView, PlayerView::draw_display);

        let vbox = self.rute.create_v_box_layout();
        let hbox = self.rute.create_h_box_layout();

        hbox.add_widget(&self.prev_button);
        hbox.add_widget(&self.stop_button);
        hbox.add_widget(&self.play_button);
        hbox.add_widget(&self.next_button);

        buttons.set_layout(&hbox);

        vbox.add_widget(&self.player_display);
        vbox.add_widget(&buttons);

        self.widget.set_layout(&vbox);
    }
}
