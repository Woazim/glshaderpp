[CmdletBinding()]
param (
    [Parameter(Mandatory=$true,
               HelpMessage="Deploy token user name.")]
    [ValidateNotNullOrEmpty()]
    [string[]]
    $User,
    [Parameter(Mandatory=$true,
    HelpMessage="Deploy token value.")]
    [ValidateNotNullOrEmpty()]
    [string[]]
    $Token,
    [Parameter(Mandatory=$true,
    HelpMessage="Release version in semver.")]
    [ValidateNotNullOrEmpty()]
    [string[]]
    $Version
)

curl -L https://gitlab-lepuy.iut.uca.fr/opengl/glshaderpp/-/jobs/artifacts/master/download?job=docker-deploy --output glshaderpp-install.zip
curl --user "${User}:$Token" --upload-file .\glshaderpp-install.zip "https://gitlab-lepuy.iut.uca.fr/api/v4/projects/345/packages/generic/release/$Version/glshaderpp-install.zip"
