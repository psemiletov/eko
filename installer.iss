; installer.iss
; Inno Setup Script for EKO Audio Editor

[Setup]
AppName=EKO
AppVersion=7.1.0
AppPublisher=EKO Team
AppPublisherURL=https://github.com/yourusername/eko
AppSupportURL=https://github.com/yourusername/eko
AppUpdatesURL=https://github.com/yourusername/eko
DefaultDirName={pf}\EKO
DefaultGroupName=EKO
UninstallDisplayIcon={app}\eko.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
OutputBaseFilename=eko-setup
SetupIconFile=icons\eko.ico
WizardStyle=modern
PrivilegesRequired=admin
AllowNoIcons=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Files]
Source: "build\eko.exe"; DestDir: "{app}"
Source: "build\*.dll"; DestDir: "{app}"; Flags: recursesubdirs
Source: "build\platforms\*"; DestDir: "{app}\platforms"
Source: "build\styles\*"; DestDir: "{app}\styles"
Source: "build\imageformats\*"; DestDir: "{app}\imageformats"
Source: "build\iconengines\*"; DestDir: "{app}\iconengines"
Source: "build\audio\*"; DestDir: "{app}\audio"

[Icons]
Name: "{group}\EKO"; Filename: "{app}\eko.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall EKO"; Filename: "{uninstallexe}"
Name: "{autodesktop}\EKO"; Filename: "{app}\eko.exe"; WorkingDir: "{app}"

[Run]
Filename: "{app}\eko.exe"; Description: "{cm:LaunchProgram,EKO}"; Flags: postinstall nowait skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Registry]
Root: HKCU; Subkey: "Software\EKO"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\EKO\Settings"; Flags: uninsdeletekey
