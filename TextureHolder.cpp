#include "TextureHolder.hpp"

// Iclude the "assert feature"
#include <assert.h>
#include <iostream>


TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder() {
    assert(m_s_Instance == nullptr);
    m_s_Instance = this;
}

sf::Texture& TextureHolder::GetTexture(const std::string &filename) {

    // Get a reference to m_Textures using m_s_Instance
    //  auto is map<std::string, sf::Texture>
    auto& m = m_s_Instance->m_Textures; // m holds the address of m_Textures

    // Create an iterator (key-value) and search using the filename passed
    // auto is map<std::string, sf::Texture>::iterator
    auto keyVal = m.find(filename);

    // Match found?
    if (keyVal != m.end()) {
        // Yes: return texture
        return keyVal->second; // second is the texture
    }
    else {
        // No: create new key-value pair using filename
        // Gets the address of second (sf::Texture) into texture
        auto& texture = m[filename]; 
        
        // Load an image to that adress
        if (!texture.loadFromFile(filename))
            std::cerr << "Error loading texture.";

        // Return the texture
        return texture;
    }
}