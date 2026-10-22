param(
 [Parameter(Mandatory=$true)][string]$ProfileStorage,
 [ValidateSet('read','stop','respawn')][string]$Operation='read',
 [ValidateSet('bay','drive','service','range','damage','hostile')][string]$Station='bay'
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(!(Test-Path -LiteralPath $ProfileStorage -PathType Container)){throw 'Use the existing isolated run profile storage directory; no profile is created here.'}
# FileIO $profile: is often the launch profile's nested profile directory.
# The coordinator records the actual storage root in the owner receipt.
$sequence=[Guid]::NewGuid().ToString('N')
$commandPath=Join-Path $ProfileStorage 'GPG-command.txt'
[IO.File]::WriteAllText($commandPath,($sequence+'|'+$Operation+'|'+$Station),[Text.UTF8Encoding]::new($false))
Write-Output ('Command submitted: '+$commandPath+'; inspect GPG_COMMAND/GPG_RESPAWN/GPG_HOSTILE in native log. Submission does not prove execution.')
