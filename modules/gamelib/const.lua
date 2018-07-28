-- @docconsts @{

FloorHigher = 0
FloorLower = 15

GenderNone = 0
GenderUndefined = 1
GenderMale = 2
GenderFemale = 3

ShieldNone = 0
ShieldWhiteYellow = 1
ShieldWhiteBlue = 2
ShieldBlue = 3
ShieldYellow = 4
ShieldBlueSharedExp = 5
ShieldYellowSharedExp = 6
ShieldBlueNoSharedExpBlink = 7
ShieldYellowNoSharedExpBlink = 8
ShieldBlueNoSharedExp = 9
ShieldYellowNoSharedExp = 10
ShieldGray = 11

EmblemNone = 0
EmblemGreen = 1
EmblemRed = 2
EmblemBlue = 3
EmblemMember = 4
EmblemOther = 5

VipIconFirst = 0
VipIconLast = 10

Directions = {
  North = 0,
  East = 1,
  South = 2,
  West = 3,
  NorthEast = 4,
  SouthEast = 5,
  SouthWest = 6,
  NorthWest = 7
}

Skill = {
  Fist = 0,
  Club = 1,
  Sword = 2,
  Axe = 3,
  Distance = 4,
  Shielding = 5,
  Fishing = 6
}

North = Directions.North
East = Directions.East
South = Directions.South
West = Directions.West
NorthEast = Directions.NorthEast
SouthEast = Directions.SouthEast
SouthWest = Directions.SouthWest
NorthWest = Directions.NorthWest

FightOffensive = 1
FightBalanced = 2
FightDefensive = 3

DontChase = 0
ChaseOpponent = 1

PVPWhiteDove = 0
PVPWhiteHand = 1
PVPYellowHand = 2
PVPRedFist = 3

GamePlayerMounts = 12
GameMagicEffectU16 = 16
GameChargeableItems = 19
-- 20 is unused
-- 23 is unused
GameChangeMapAwareRange = 30
GameMapMovePosition = 31
GameBlueNpcNameColor = 33
GameDiagonalAnimatedText = 34
GameForceFirstAutoWalkStep = 37
GameMinimapRemove = 38
-- empty
GameMesssageLevel = 46
GameSpritesAlphaChannel = 56
GameAuthenticator = 67
GameKeepUnawareTiles = 72
GameIngameStore = 73

TextColors = {
  red       = '#f55e5e', --'#c83200'
  orange    = '#f36500', --'#c87832'
  yellow    = '#ffff00', --'#e6c832'
  green     = '#00EB00', --'#3fbe32'
  lightblue = '#5ff7f7',
  blue      = '#9f9dfd',
  --blue1     = '#6e50dc',
  --blue2     = '#3264c8',
  --blue3     = '#0096c8',
  white     = '#ffffff', --'#bebebe'
}

MessageModes = {
  None                    = 0,
  Say                     = 1,
  Whisper                 = 2,
  Yell                    = 3,
  PrivateFrom             = 4,
  PrivateTo               = 5,
  ChannelManagement       = 6,
  Channel                 = 7,
  ChannelHighlight        = 8,
  Spell                   = 9,
  NpcFrom                 = 10,
  NpcTo                   = 11,
  GamemasterBroadcast     = 12,
  GamemasterChannel       = 13,
  GamemasterPrivateFrom   = 14,
  GamemasterPrivateTo     = 15,
  Login                   = 16,
  Warning                 = 17,
  Game                    = 18,
  Failure                 = 19,
  Look                    = 20,
  DamageDealed            = 21,
  DamageReceived          = 22,
  Heal                    = 23,
  Exp                     = 24,
  DamageOthers            = 25,
  HealOthers              = 26,
  ExpOthers               = 27,
  Status                  = 28,
  Loot                    = 29,
  TradeNpc                = 30,
  Guild                   = 31,
  PartyManagement         = 32,
  Party                   = 33,
  BarkLow                 = 34,
  BarkLoud                = 35,
  Report                  = 36,
  HotkeyUse               = 37,
  TutorialHint            = 38,
  Thankyou                = 39,
  Market                  = 40,
  Mana                    = 41,
  BeyondLast              = 42,
  PokemonYell             = 43,
  PokemonSay              = 44,
  Red                     = 45,
  Blue                    = 46,
  RVRChannel              = 47,
  RVRAnswer               = 48,
  RVRContinue             = 49,
  GameHighlight           = 50,
  NpcFromStartBlock       = 51,
  Last                    = 52,
  Invalid                 = 255,
}

RUBYSERV_RSA  = "1309487737846324944857898052184048155143986592147764827838444451" ..
                "1272444903750171047438233915528135496382406190348745751653837769" ..
                "1170532971384892430706580437505229149151501715843684726811426812" ..
                "2573842672020364222682681476802796616428296935458948896081391664" ..
                "70255591072814985834514077964713656055651898409836961"

-- set to the latest Tibia.pic signature to make otclient compatible with official tibia
PIC_SIGNATURE = 0x56C5DDE7

OsTypes = {
  Linux = 1,
  Windows = 2,
  Flash = 3,
  OtclientLinux = 10,
  OtclientWindows = 11,
  OtclientMac = 12,
}

PathFindResults = {
  Ok = 0,
  Position = 1,
  Impossible = 2,
  TooFar = 3,
  NoWay = 4,
}

PathFindFlags = {
  AllowNullTiles = 1,
  AllowCreatures = 2,
  AllowNonPathable = 4,
  AllowNonWalkable = 8,
}

VipState = {
  Offline = 0,
  Online = 1,
  Pending = 2,
}

ExtendedIds = {
  Activate = 0,
  Locale = 1,
  Ping = 2,
  Sound = 3,
  Game = 4,
  Particles = 5,
  MapShader = 6,
  NeedsUpdate = 7
}

PreviewState = {
  Default = 0,
  Inactive = 1,
  Active = 2
}

Blessings = {
  None = 0,
  Adventurer = 1,
  SpiritualShielding = 2,
  EmbraceOfTibia = 4,
  FireOfSuns = 8,
  WisdomOfSolitude = 16,
  SparkOfPhoenix = 32
}

DeathType = {
  Regular = 0,
  Blessed = 1
}

ProductType = {
  Other = 0,
  NameChange = 1
}

StoreErrorType = {
  NoError = -1,
  PurchaseError = 0,
  NetworkError = 1,
  HistoryError = 2,
  TransferError = 3,
  Information = 4
}

StoreState = {
  None = 0,
  New = 1,
  Sale = 2,
  Timed = 3
}

-- @}
