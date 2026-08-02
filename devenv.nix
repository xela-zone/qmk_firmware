{
  pkgs,
  lib,
  config,
  inputs,
  ...
}:

{
  # Define the packages to be installed in the environment
  packages = with pkgs; [
    qmk
    gcc-arm-embedded
    dfu-util
    dfu-programmer
    git
    clang-tools
  ];

  scripts = {
    build-xela.exec = "qmk compile -kb keychron/q3_max/ansi_encoder -km xela --compiledb";
    flash.exec = ''
      qmk flash -kb keychron/q3_max/ansi_encoder -km xela
      echo "Waiting 5 seconds before reflashing default binary..."
      sleep 5
      qmk flash keychron_q3_max_ansi_encoder_default.bin
    '';
  };

  # Enter hook to set up environment variables or run scripts when shell starts
  enterShell = ''
    echo "========================================="
    echo "  QMK Firmware Custom Dev Environment    "
    echo "========================================="
    echo "Available custom commands:"
    echo "  build-xela - Compiles the Q3 Max keymap"
    echo "  flash      - Compiles and flashes keymap, then offers to reflash default bin"
    echo "========================================="
    qmk --version
  '';
}
