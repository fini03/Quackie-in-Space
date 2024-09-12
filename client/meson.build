sdl2 = dependency('sdl2')
sdl2_mixer = dependency('sdl2_mixer')
ffmpeg_client = [
  dependency('libavcodec'),
  dependency('libavutil'),
]

executable(
  'client',
  sources: [
    'Client.cpp',
    'Network.cpp',
    'Decoder.cpp',
    'StreamReceiver.cpp',
  ],
  include_directories: ['../shared'],
  dependencies: [ffmpeg_client, sdl2, sdl2_mixer]
)
