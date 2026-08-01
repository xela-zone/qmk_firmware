{ pkgs, lib, config, inputs, ... }:

{
  # Define the packages to be installed in the environment
  packages = with pkgs; [
    qmk
    gcc-arm-embedded
    dfu-util
    dfu-programmer
    git
  ];

  # Custom scripts (aliases) available inside the environment
  scripts = {
    build-xela.exec = "qmk compile -kb keychron/q3_max/ansi_encoder -km xela";
    flash-xela.exec = "qmk flash -kb keychron/q3_max/ansi_encoder -km xela";
  };

  # Enter hook to set up environment variables or run scripts when shell starts
  enterShell = ''
    echo "========================================="
    echo "  QMK Firmware Custom Dev Environment    "
    echo "========================================="
    echo "Available custom commands:"
    echo "  build-xela - Compiles the Q3 Max keymap"
    echo "  flash-xela - Compiles and flashes keymap"
    echo "========================================="
    qmk --version
  '';
}
