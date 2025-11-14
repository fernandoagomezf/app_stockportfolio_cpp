[Setup]
AppName=Stock Portfolio Tracker
AppVersion=0.1.0
AppPublisher=Blendwerk
AppPublisherURL=https://github.com/fernandoagomezf/app_stockportfolio_cpp
DefaultDirName={autopf}\Stock Portfolio Tracker
DefaultGroupName=Blendwerk
UninstallDisplayIcon={app}\SPT.exe
Compression=lzma2
SolidCompression=yes
OutputDir=..\build
OutputBaseFilename=SPT-v0.1.1
WizardStyle=modern
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "..\build\msvc\Release\SPT.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\msvc\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Stock Portfolio Tracker"; Filename: "{app}\SPT.exe"
Name: "{group}\{cm:UninstallProgram,Stock Portfolio Tracker}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Stock Portfolio Tracker"; Filename: "{app}\SPT.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\SPT.exe"; Description: "{cm:LaunchProgram,Stock Portfolio Tracker}"; Flags: nowait postinstall skipifsilent

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Add any post-install tasks here
  end;
end;