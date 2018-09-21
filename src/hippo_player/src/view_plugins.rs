/*
impl ViewPlugin {
	pub fn get_name<'a>(&'a self) -> Cow<'a, str> {
        //let res = unsafe { self.plugin_funcs.name };
        unsafe { CStr::from_ptr(self.plugin_funcs.name as *const i8).to_string_lossy() }
	}

	pub fn create_instance(&self, ui: &Ui, plugin_service: &PluginService, window: &Widget) -> ViewPluginInstance {
        let plugin_ui = ui.create_plugin_ui(window);

		let user_data = ((self.plugin_funcs).create).unwrap()(
			plugin_service.get_c_service_api());

		((self.plugin_funcs).setup_ui).unwrap()(user_data, plugin_ui.get_c_api());

		ViewPluginInstance {
			plugin: self.clone(),
			user_data: user_data as u64,
			id: self.count,
			ui: plugin_ui,
		}
	}
}
*/

