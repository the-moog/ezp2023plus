project('libadwaita', 'c',
        version: '1.6.alpha',
        license: 'GPL-2.0',
        meson_version: '>= 0.59.0',
        default_options: [ 'warning_level=1', 'buildtype=debugoptimized', 'c_std=gnu11' ],
)

version_arr = meson.project_version().split('-')[0].split('.')
adwaita_version_major = version_arr[0].to_int()
adwaita_version_minor = version_arr[1].to_int()

if version_arr[2].startswith('alpha') or version_arr[2].startswith('beta') or version_arr[2].startswith('rc')
  adwaita_version_micro = 0
else
  adwaita_version_micro = version_arr[2].to_int()
endif

# The major api version as encoded in the libraries name
apiversion = adwaita_version_major.to_string()
package_api_name = '@0@-@1@'.format(meson.project_name(), apiversion)


gnome = import('gnome')

# Setup various paths that subdirectory meson.build files need
package_subdir = get_option('package_subdir') # When used as subproject
if package_subdir != ''
  datadir    = get_option('datadir') / package_subdir
  libdir     = get_option('libdir')  / package_subdir
  girdir     = get_option('datadir') / package_subdir / 'gir-1.0'
  typelibdir = get_option('libdir')  / package_subdir / 'girepository-1.0'
  vapidir    = get_option('datadir') / package_subdir / 'vapi'
else
  datadir    = get_option('datadir')
  libdir     = get_option('libdir')
  girdir     = get_option('datadir') / 'gir-1.0'
  typelibdir = get_option('libdir')  / 'girepository-1.0'
  vapidir    = get_option('datadir') / 'vala' / 'vapi'
endif

subdir('po')
subdir('data')

demo_config_data = configuration_data()
demo_config_data.set_quoted('ADW_METAINFO', appstream_file.full_path())
demo_config_data.set_quoted('ADW_DEMO_VCS_TAG', '@VCS_TAG@')

demo_config_h = vcs_tag(
  input: configure_file(
    output: 'config.h.in',
    configuration: demo_config_data
  ),
  output: 'config.h'
)

adwaita_demo_resources = gnome.compile_resources(
   'adwaita-demo-resources',
   'adwaita-demo.gresources.xml',

   c_name: 'adw',
   dependencies: appstream_file,
   source_dir: meson.current_build_dir(),
)

adwaita_demo_sources = [
  adwaita_demo_resources,

  'pages/about/adw-demo-page-about.c',
  'pages/animations/adw-demo-page-animations.c',
  'pages/avatar/adw-demo-page-avatar.c',
  'pages/banners/adw-demo-page-banners.c',
  'pages/buttons/adw-demo-page-buttons.c',
  'pages/carousel/adw-demo-page-carousel.c',
  'pages/clamp/adw-demo-page-clamp.c',
  'pages/dialogs/adw-demo-page-dialogs.c',
  'pages/lists/adw-demo-page-lists.c',
  'pages/navigation-view/adw-demo-page-navigation-view.c',
  'pages/navigation-view/adw-navigation-view-demo-window.c',
  'pages/split-views/adw-demo-page-split-views.c',
  'pages/split-views/adw-navigation-split-view-demo-window.c',
  'pages/split-views/adw-overlay-split-view-demo-window.c',
  'pages/styles/adw-demo-page-styles.c',
  'pages/styles/adw-style-demo-window.c',
  'pages/tab-view/adw-demo-page-tab-view.c',
  'pages/tab-view/adw-tab-view-demo-page.c',
  'pages/tab-view/adw-tab-view-demo-window.c',
  'pages/toasts/adw-demo-page-toasts.c',
  'pages/view-switcher/adw-demo-page-view-switcher.c',
  'pages/view-switcher/adw-view-switcher-demo-window.c',
  'pages/welcome/adw-demo-page-welcome.c',

  'adwaita-demo.c',
  'adw-demo-debug-info.c',
  'adw-demo-preferences-window.c',
  'adw-demo-window.c',
  demo_config_h,
]

libadwaita_dep = dependency('libadwaita-1', version: '>= 1.2')

adwaita_demo = executable('adwaita-@0@-demo'.format(apiversion),
  adwaita_demo_sources,
  dependencies: libadwaita_dep,
  install: true,
)