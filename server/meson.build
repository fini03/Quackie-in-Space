box2d = dependency('box2d')
tmxlite = dependency('tmxlite')
ffmpeg_server = [
  dependency('libavcodec'),
  dependency('libswscale'),
  dependency('libavutil'),
]

src = [
  'main.cpp',

  'blocks/Block.cpp',
  'blocks/BlockManager.cpp',
  'blocks/Button.cpp',
  'blocks/ButtonManager.cpp',
  'blocks/FragileBlock.cpp',
  'blocks/FragileBlockManager.cpp',
  'blocks/Platform.cpp',
  'blocks/PlatformManager.cpp',
  'blocks/Slide.cpp',
  'blocks/SlideManager.cpp',
  'blocks/Tile.cpp',
  'blocks/TileManager.cpp',

  'eventlisteners/Physics.cpp',
  'eventlisteners/CollisionListener.cpp',
  'eventlisteners/InterpolatedObject.cpp',
  'eventlisteners/Keys.cpp',
  'eventlisteners/GUI.cpp',

  'game/GameManager.cpp',
  'game/Player.cpp',
  'game/Goal.cpp',
  'game/Key.cpp',
  'game/MyVulkanEngine.cpp',
  'game/Floor.cpp',

  'parser/TMXParser.cpp',

  'traps/Fire.cpp',
  'traps/LaserBeam.cpp',
  'traps/Chainsaw.cpp',
  'traps/Trap.cpp',
  'traps/TrapManager.cpp',

  'stream/Stream.cpp',
  'stream/Encoder.cpp',
  'stream/UDPSend.cpp',
  'stream/EventReceiver.cpp'
]

executable(
  'server',
  sources: src,
  include_directories: ['../shared'],
  dependencies: [vve, tmxlite, box2d, ffmpeg_server]
)
