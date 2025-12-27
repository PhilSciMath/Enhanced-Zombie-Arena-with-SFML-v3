#pragma once
#include <string>
#ifndef TEXTURE_HOLDER_HPP
#define TEXTURE_HOLDER_HPP

#include <SFML/Graphics.hpp>
#include <map>


class TextureHolder {
    private:
        // A map container from STL holding pairs of String and Texture
        std::map<std::string, sf::Texture> m_Textures;

        // A pointer of the same type of the class itself, the one and only
        // instance (singleton)
        static TextureHolder* m_s_Instance;

    public:
        TextureHolder();
        static sf::Texture& GetTexture(std::string const& filename);
};

#endif