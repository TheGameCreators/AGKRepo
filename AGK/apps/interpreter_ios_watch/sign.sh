#! /bin/sh
# copy provisioning profile to .app
# edit entitlements.xcent

/usr/sbin/chown -RH $(id -u -n):$(id -g -n) "Export/AGK 2 Player.app"
/bin/chmod -RH u+w,go-w,a+rX "Export/AGK 2 Player.app"

IDENTITY=2893CEB913DE3EB786C70DF3816A4DB854CDD2EB

codesign --force --sign B7A0333DE0E9C60C5200F148D177DDEC7F87280A --resource-rules "Export/AGK 2 Player.app/ResourceRules.plist" --entitlements "Export/entitlements.xcent" "Export/AGK 2 Player.app"