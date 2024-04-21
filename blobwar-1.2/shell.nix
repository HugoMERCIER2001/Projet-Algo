{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    SDL
    SDL_image
    SDL_ttf
    SDL_net
    gcc
    clang-tools
  ];
}
