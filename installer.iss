; installer.iss
; Inno Setup Script for EKO Audio Editor (MinGW build)

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
; SetupIconFile убран навсегда
WizardStyle=modern
PrivilegesRequired=admin
AllowNoIcons=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Files]
Source: "installer_files\eko.exe"; DestDir: "{app}"
Source: "installer_files\*.dll"; DestDir: "{app}"; Flags: recursesubdirs
Source: "installer_files\platforms\*"; DestDir: "{app}\platforms"
Source: "installer_files\styles\*"; DestDir: "{app}\styles"
Source: "installer_files\imageformats\*"; DestDir: "{app}\imageformats"

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
