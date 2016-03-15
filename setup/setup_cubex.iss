#define   Name       "Cubex"
#define   Version    "1.2.0"
#define   Publisher  "Alexander Gaivanuk"
#define   ExeName    "Cubex.exe"
#define   URL        "https://github.com/alexgaiv/"

[Setup]
AppId={{B4D97D4B-0510-47AB-9EE3-594E2A482131}

AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}

DefaultDirName={pf}\{#Name}
DefaultGroupName={#Name}
UninstallDisplayName={#Name}
UninstallDisplayIcon={app}\{#ExeName}
OutputDir="."
OutputBaseFileName="setup_cubex_{#Version}"
SetupIconFile=app.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl";
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl";

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "..\release\cubex.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\glew32.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
Name: {group}\{cm:UninstallProgram,{#Name}}; Filename: {uninstallexe}
Name: "{commondesktop}\{#Name}"; Filename: "{app}\{#ExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#ExeName}"; Description: {cm:LaunchProgram, {#Name}}; Flags: postinstall

[UninstallDelete]
Type: filesandordirs; Name: "{userappdata}\Cubex"