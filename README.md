# SwitchVerifier [![Latest release](https://img.shields.io/github/v/release/LotP1/SwitchVerifier)](https://github.com/LotP1/SwitchVerifier/releases/latest) [![Github latest downloads](https://img.shields.io/github/downloads/LotP1/SwitchVerifier/total.svg)](https://github.com/LotP1/SwitchVerifier/releases/latest)

This tool uses information from your Nintendo Switch device to generate a token that can be used to verify yourself as a real switch owner.<br>
Use it with a Discord App/Bot in the supported Discord servers to verify yourself.

# Features:
 - generates a token based on hardware values and a user hash
 - the token is also written to a file for easy copy/paste with ftp or usb file transfer
 - doesn't use any internet connection to generate the token
 - currently works with these discord servers:
   - [Ryubing](https://discord.gg/ryujinx)
   - [Zep's Zen Zone](https://discord.gg/yB7d72xhKb)
   - (contact me on discord `lotp` if you would like to add this to your server)

# Credits:
- SwitchBrew wiki for documenting various service API uses in SwitchVerifier.
- devkitPro for the devkitA64 toolchain and libnx support.
