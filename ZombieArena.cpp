#include "ZombieArena.hpp"
#include "Player.hpp"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include "TextureHolder.hpp"
#include <random>
#include <sstream>
#include <fstream>
#include "Bullet.hpp"
#include "Pickup.hpp"
#include <cmath>


const float PLAYER_RADIUS = 60.0f; // Used for circular collision detection
std::mt19937 rng(std::random_device{}());


int main() {

    // The only instance of TextureHolder
    TextureHolder holder;

    // The game is always in one of four states
    enum class State {PLAYING, PAUSED, LEVELING_UP, GAME_OVER};

    // Start with the gameover state
    State state = State::GAME_OVER;

    // Get screen resolution and create an SFML window
    sf::VideoMode vm = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(vm, "Zombie Arena", sf::State::Fullscreen);
    sf::Vector2f resolution(vm.size.x, vm.size.y); // Useful later

    // Create an SFML view for the main action
    sf::View mainView(sf::Vector2f(resolution.x / 2.0f, resolution.y / 2.0f), 
    sf::Vector2f(resolution.x, resolution.y));
    

    // A clock for general use
    sf::Clock clock;

    // How long has the playing state being active
    sf::Time gameTimeTotal;
    
    // Mouse position with respecto to the world
    sf::Vector2f mouseWorldPosition;

    // Mouse position on the screen
    sf::Vector2i mouseScreenPosition;

    // Create the player
    Player player;

    // Boundaries of the arena
    sf::IntRect arena;

    // Create background and texture
    sf::VertexArray background;
    sf::Texture backgroundTexture(TextureHolder::GetTexture(
        "graphics/background.png"));       
    
    // Prepare the Horde
    int numZombies;
    int numZombiesAlive;
    Zombie* zombies = nullptr;

    // Preparing bullets, about 100 
    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 1.0f;
    sf::Time lastPressed; // last time the trigger was pressed

    // Hide the mouse pointer and replace it with a crosshair
    window.setMouseCursorVisible(false);
    sf::Sprite crossHair(TextureHolder::GetTexture("graphics/crosshair.png"));
    crossHair.setScale({0.1f, 0.1f});
    //crossHair.setRotation(sf::degrees(45));
    crossHair.setOrigin({crossHair.getLocalBounds().size / 2.0f});

    // Create pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);

    // Score variables
    int score = 0;
    int hiScore = 0;

    // Background for home/gameover screen
    sf::Sprite gameOverSprite(TextureHolder::GetTexture(
        "graphics/gameover.png"));
    gameOverSprite.setPosition({0, 0});

    // A view for the HUD
    sf::View hudView({resolution.x / 2, resolution.y / 2},
        {resolution.x, resolution.y});

    // Sprite for the ammo icon
    sf::Sprite ammoIcon(TextureHolder::GetTexture(
        "graphics/ammo_icon.png"));      
    ammoIcon.setScale({0.1f, 0.1f});    
    ammoIcon.setPosition({80, resolution.y - 120});

    // Load a font for the HUD
    sf::Font font("fonts/RobotoCondensed-Medium.ttf");

    // Paused HUD message
    sf::Text pausedMessage(font);
    pausedMessage.setCharacterSize(155);
    pausedMessage.setFillColor(sf::Color::White);
    pausedMessage.setPosition({400, 400});
    pausedMessage.setString("Press Enter \nto continue!");

    // Game over HUD message
    sf::Text gameOverMessage(font);
    gameOverMessage.setCharacterSize(125);
    gameOverMessage.setFillColor(sf::Color::White);
    gameOverMessage.setPosition({250, 850});
    gameOverMessage.setString("Press Enter to play");

    // Leveling up HUD message
    sf::Text levelUpMessage(font);
    levelUpMessage.setCharacterSize(80);
    levelUpMessage.setFillColor(sf::Color::White);
    levelUpMessage.setPosition({150, 250});
    std::stringstream levelUpStream;
    levelUpStream << 
        "1 - Increased rate of fire" <<
        "\n2 - Increased clipsize (next reload)" <<
        "\n3 - Increased max health" <<
        "\n4 - Increased run speed" <<
        "\n5 - More and better health pickups" <<
        "\n6 - More and better ammo pickups";
    levelUpMessage.setString(levelUpStream.str());

    // Ammo HUD info
    sf::Text ammoText(font);
    ammoText.setCharacterSize(55);
    ammoText.setFillColor(sf::Color::White);
    ammoText.setPosition({200, 980});

    // Score HUD info
    sf::Text scoreText(font);
    scoreText.setCharacterSize(55);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20, 0});

    // Load Hi Score from a txt file
    std::ifstream file("gamedata/score.txt");
    if (file.is_open()) {
        file >> hiScore;
        file.close();
    }

    // HiScore HUD info
    sf::Text hiScoreText(font);
    hiScoreText.setCharacterSize(55);
    hiScoreText.setFillColor(sf::Color::White);
    hiScoreText.setPosition({1400, 0});
    std::stringstream hiScoreString;
    hiScoreString << "Hi Score: " << hiScore;
    hiScoreText.setString(hiScoreString.str());

    // Zombies remaining info
    sf::Text zombiesRemainingText(font);
    zombiesRemainingText.setCharacterSize(55);
    zombiesRemainingText.setFillColor(sf::Color::White);
    zombiesRemainingText.setPosition({1500, 980});
    zombiesRemainingText.setString("Zombies: 100");

    // Wave number info
    int wave = 0;
    sf::Text waveNumberText(font);
    waveNumberText.setCharacterSize(55);
    waveNumberText.setFillColor(sf::Color::White);
    waveNumberText.setPosition({1250, 980});
    waveNumberText.setString("Wave: 0");

    // Health bar 
    sf::RectangleShape healthBar;
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition({450, 980});
    
    // Last time the HUD was updated
    int framesSinceHUDUpdate = 0;
    // How often should the HUD be updated
    int fpsMeasurementFrameInterval = 1000;

    // ---- SOUND ------
    // Hit sound
    sf::SoundBuffer hitBuffer("sounds/ouch.ogg");
    sf::Sound hitSound(hitBuffer);

    // Splat (zombie die)
    sf::SoundBuffer splatBuffer("sounds/splat.ogg");
    sf::Sound splatSound(splatBuffer);

    // Gun shot
    sf::SoundBuffer gunBuffer("sounds/gun.ogg");
    sf::Sound gunSound(gunBuffer);

    // Reload success
    sf::SoundBuffer reloadBuffer("sounds/reload.ogg");
    sf::Sound reloadSound(reloadBuffer);

    // Reload failed
    sf::SoundBuffer failedBuffer("sounds/reload_failed.ogg");
    sf::Sound failedSound(failedBuffer);

    // Power up
    sf::SoundBuffer powerUpBuffer("sounds/levelup.ogg");
    sf::Sound powerUpSound(powerUpBuffer);

    // Pickup sound
    sf::SoundBuffer pickupBuffer("sounds/pickup.ogg");
    sf::Sound pickUpSound(pickupBuffer);

    // Zombie sound
    sf::SoundBuffer zombieSoundBuffer1("sounds/zombie_sound_1.ogg");
    sf::Sound zombieSound1(zombieSoundBuffer1);
    sf::SoundBuffer zombieSoundBuffer2("sounds/zombie_sound_2.ogg");
    sf::Sound zombieSound2(zombieSoundBuffer2);

    // Add some variations randomly during PLAYING state
    float zombieSoundLastTimePlayed = 0.0f;
    std::uniform_int_distribution<> zombieSoundChance(1, 20); 

    // Player footsteps sound
    sf::SoundBuffer playerStepBuffer("sounds/footsteps.ogg");
    sf::Sound playerStepSound(playerStepBuffer);
    playerStepSound.setLooping(true);  
    bool playerIsMoving = false; // Stop sound if not moving   
    bool wasPlayerMoving = false; // Tricky but needed 
   

    // Main game loop
    while (window.isOpen()) {

        /*
        ************
        HANDLE INPUT
        ************
        */

        // Handle events by polling
        while (std::optional<sf::Event> event = window.pollEvent()) {

            bool isKeyPressed = event->is<sf::Event::KeyPressed>();

            // Window close or escape key to exit
            if (event->is<sf::Event::Closed>() || 
                (isKeyPressed && event->getIf<sf::Event::KeyPressed>()->code ==
                sf::Keyboard::Key::Escape))
            {
                window.close();
            }

            bool isEnterPressed = event->is<sf::Event::KeyPressed>() &&
                event->getIf<sf::Event::KeyPressed>()->code ==
                sf::Keyboard::Key::Enter;

            // Pause the game while playing
            if (isEnterPressed && state == State::PLAYING) {
                state = State::PAUSED;
            }

            // Restart while paused
            else if (isEnterPressed && state == State::PAUSED) {
                state = State::PLAYING;
                clock.restart(); // So there is not frame jump
            }

            // Start a new game while in game over state
            else if (isEnterPressed && state == State::GAME_OVER) {
                state = State::LEVELING_UP;

                wave = 0;
                score = 0;

            // Prepare gun and ammo for next game
                currentBullet = 0;
                bulletsSpare = 24;
                clipSize = 6;
                bulletsInClip = 6;
                fireRate = 1;

                // Reset player stats
                player.resetPlayerStats();
            }
            
            // Handle the leveling up state
            if (state == State::LEVELING_UP) {

                // Handle the player leveling up
                if (isKeyPressed) {
                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num1) 
                    {
                        // Increase fire rate
                        fireRate++;
                        state = State::PLAYING;
                    }

                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num2) 
                    {
                        // Increase clip size
                        clipSize += clipSize;
                        state = State::PLAYING;
                    }

                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num3) 
                    {
                        // Health increase
                        player.upgradeHealth();
                        state = State::PLAYING;
                    }

                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num4) 
                    {
                        // Increase speed
                        player.upgradeSpeed();
                        state = State::PLAYING;
                    }

                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num5) 
                    {
                        // Upgrade health pickup
                        healthPickup.upgrade();
                        state = State::PLAYING;
                    }

                    if (event->getIf<sf::Event::KeyPressed>()->code 
                        == sf::Keyboard::Key::Num6) 
                    {
                        // Upgrade ammo 
                        ammoPickup.upgrade();
                        state = State::PLAYING;
                    }

                    // Preparing the level
                    if (state == State::PLAYING) {

                        wave++;
                        
                        arena = sf::IntRect({0, 0}, {800 * wave,800 * wave});                        

                        // Pass vertex array background by reference to 
                        // createBackground(), which returns the tile size
                        int tileSize = createBackground(background, arena);

                        // Spawn the player in the middle of the arena
                        player.spawn(arena, resolution, tileSize);                    
                        clock.restart(); // Avoid frame jump

                        // Configure the pickups
                        healthPickup.setArena(arena);
                        ammoPickup.setArena(arena);

                        // Create a horde of zombies
                        numZombies = 5 * wave;

                        // Delete the previously allocated memory
                        delete[] zombies;
                        zombies = createHorde(numZombies, arena);
                        numZombiesAlive = numZombies; // none is dead yet

                        // Play the power up sound
                        powerUpSound.play();
                    }

                }                  
                
            } // End of handling leveling up state

            if (state == State::PLAYING) {
                // Reloading
                if (isKeyPressed && event->getIf<sf::Event::KeyPressed>()->code 
                    == sf::Keyboard::Key::R) {

                    if (bulletsInClip >= clipSize) {
                        // Prevents reloading when full
                        break; 
                    }                    
                    else if (bulletsSpare > 0) {
                        
                        // Amount of ammo spent - clip may not be empty
                        int ammoSpent = clipSize - bulletsInClip;

                        if (bulletsSpare >= ammoSpent) {
                            bulletsSpare -= ammoSpent;
                            bulletsInClip += ammoSpent;                          
                        }
                        else {
                            bulletsInClip += bulletsSpare;
                            bulletsSpare = 0;
                        }                        
                        reloadSound.play();
                    }                 
                    else {
                        // Failed reloading
                        failedSound.play();
                    }
                }
            }

        } // End event polling

        // Handle WASD while playing
        if (state == State::PLAYING) { 
            
            playerIsMoving = false;
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                player.moveUp();   
                playerIsMoving = true;                           
            }
            else {
                player.stopUp();                                
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                player.moveDown(); 
                playerIsMoving = true;                             
            }
            else {
                player.stopDown();                               
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                player.moveLeft();   
                playerIsMoving = true;                             
            }
            else {
                player.stopLeft();                              
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                player.moveRight(); 
                playerIsMoving = true;               
            }
            else {
                player.stopRight();                               
            }            

            // Fire a bullet            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                
                // No bullets in clip? 
                if (bulletsInClip < 1) {
                    failedSound.play();                    
                }

                // 1000 because 1 second = 1000 milliseconds
                if (gameTimeTotal.asMilliseconds() - 
                    lastPressed.asMilliseconds() > 1000 / fireRate
                    && bulletsInClip > 0) {
                    
                    // Shoot
                    bullets[currentBullet].shoot(
                        player.getCenter().x, player.getCenter().y, 
                        mouseWorldPosition.x, mouseWorldPosition.y);

                    currentBullet++;
                    if (currentBullet > 99) {
                        currentBullet = 0;
                    }
                    lastPressed = gameTimeTotal;
                    gunSound.play();
                    bulletsInClip--;
                }
                
            } // End firing a bullet
        } // End handling WASD


        /*
        ****************
        UPDATE THE FRAME
        ****************
        */
        if (state == State::PLAYING) {

            // Update delta time
            sf::Time dt = clock.restart();

            // Update the total game time
            gameTimeTotal += dt;

            // Make a decimal fraction of 1 from the delta time
            float dtAsSeconds = dt.asSeconds();

            // Where is the mouse pointer
            mouseScreenPosition = sf::Mouse::getPosition(window);            

            // Convert mouse screen position to world coordinates of mainView
            mouseWorldPosition = window.mapPixelToCoords(
                mouseScreenPosition, mainView);
            
            // Set the crosshair to the mouse world location
            crossHair.setPosition(mouseWorldPosition);
            
            // Update player
            player.update(dtAsSeconds, 
                static_cast<sf::Vector2i>(mouseWorldPosition));

            // Make a note of the player's new position
            sf::Vector2f playerPosition(player.getCenter());

            // Make the mainView center around the player
            mainView.setCenter(player.getCenter());

            // Loop through each zombie and update them
            for (int i = 0; i < numZombies; i ++) {
                if (zombies[i].isAlive()) {
                    zombies[i].update(dtAsSeconds, playerPosition);
                }
            }

            // Update any bullets that are in flight
            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {
                    bullets[i].update(dtAsSeconds);
                }
            }

            // Update pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);

            // Collision Detection
            // Handle zombie being shot
            for (int i = 0; i < 100; i++) {
                for (int j = 0; j < numZombies; j++ ) {

                    if (bullets[i].isInFlight() && zombies[j].isAlive()) {

                        if (bullets[i].getPosition().findIntersection(
                            zombies[j].getPosition())) {

                            // Stop bullets
                            bullets[i].stop();

                            // Register the hit, check if it was a kill
                            if (zombies[j].hit()) {

                                // Hit and kill
                                score += 10;
                                if (score >= hiScore) {
                                    hiScore = score;
                                }

                                // Decrease number of zombies alive
                                numZombiesAlive--;

                                // All zombies dead?
                                if (numZombiesAlive == 0) {
                                        state = State::LEVELING_UP;
                               }
                            }
                            splatSound.play();
                        }
                    }
                }
            } //End zombie being shot

            // Handle the player getting hit
            for (int i = 0; i < numZombies; i++) {
                if (circlesIntersect(playerPosition, 
                    zombies[i].getPosition().getCenter()) && 
                    zombies[i].isAlive()) {

                        if (player.hit(gameTimeTotal)) {
                            hitSound.play();
                        }

                        if (player.getHealth() <= 0) {
                            state = State::GAME_OVER;

                            std::ofstream file("gamedata/score.txt");
                            file << hiScore;
                            file.close();
                        }

                    }
            } // End player getting hit

            // Handle collision with pickup
            if (player.getPosition().findIntersection(
                healthPickup.getPosition()) && healthPickup.isSpawned()) {

                player.increaseHealthLevel(healthPickup.gotIt());
                pickUpSound.play();                
            }

            if (player.getPosition().findIntersection(
                ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
                
                bulletsSpare += ammoPickup.gotIt();
                reloadSound.play();
            }

            // Health bar resize
            healthBar.setSize({(float) player.getHealth() * 3, 50});

            // Increment number of frames since last update
            framesSinceHUDUpdate++;

            // Re-calculating fpsMeasurementFrameInterval
            if (framesSinceHUDUpdate > fpsMeasurementFrameInterval) {
                
                // Update HUD text
                std::stringstream ssAmmo;
                std::stringstream ssScore;
                std::stringstream ssHiScore;
                std::stringstream ssWave;
                std::stringstream ssZombiesAlive;

                ssAmmo << bulletsInClip << "/" << bulletsSpare;
                ammoText.setString(ssAmmo.str());

                ssScore << "Score: " << score;
                scoreText.setString(ssScore.str());

                ssHiScore << "Hi Score: " << hiScore;
                hiScoreText.setString(ssHiScore.str());

                ssWave << "Wave: " << wave;
                waveNumberText.setString(ssWave.str());

                ssZombiesAlive << "Zombies: " << numZombiesAlive;
                zombiesRemainingText.setString(ssZombiesAlive.str());

                framesSinceHUDUpdate = 0;
            }

            // Zombie sounds during PLAYING state
            zombieSoundLastTimePlayed++;
            if (zombieSoundLastTimePlayed > 10000) {
                int result = zombieSoundChance(rng);
                if (result > 15) {
                    zombieSound1.play();                    
                }                
                else if (result > 10) {
                    zombieSound2.play();
                }
                zombieSoundLastTimePlayed = 0;
            }

            // Handle player steps sound            
            if (playerIsMoving && !wasPlayerMoving) {
                playerStepSound.play();
            }
            else if (!playerIsMoving || state != State::PLAYING) {
                playerStepSound.pause();                                
            }  
            wasPlayerMoving = playerIsMoving;

        }// End updating the scene
        

        /*
        **************
        DRAW THE SCENE
        **************
        */
        if (state == State::PLAYING) {
            window.clear();

            // Set the mainView to be displayed, then draw everything
            // that is related to it            
            window.setView(mainView);

            // Draw background
            window.draw(background, &backgroundTexture);

            // Draw zombies
            for (int i = 0; i < numZombies; i++) {                 
                window.draw(zombies[i].getSprite());
            }

            // Draw bullets 
            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {                    
                    window.draw(bullets[i].getShape());
                }
            }

            // Draw the player
            window.draw(player.getSprite());

            // Draw pickups if spawned
            if (ammoPickup.isSpawned()) {
                window.draw(ammoPickup.getSprite());
            }

            if (healthPickup.isSpawned()) {
                window.draw(healthPickup.getSprite());
            }

            // Draw the crosshair
            window.draw(crossHair);

            // Switch to the hudView
            window.setView(hudView);

            // Draw all the HUD elements
            window.draw(ammoIcon);
            window.draw(ammoText);
            window.draw(scoreText);
            window.draw(hiScoreText);
            window.draw(healthBar);
            window.draw(waveNumberText);
            window.draw(zombiesRemainingText);
        }

        if (state == State::LEVELING_UP) {

            window.draw(gameOverSprite);
            window.draw(levelUpMessage);
        }

        if (state == State::PAUSED) {
            
            window.draw(gameOverSprite);
            window.draw(pausedMessage);
        }

        if (state == State::GAME_OVER) {

            window.draw(gameOverSprite);
            window.draw(gameOverMessage);
            window.draw(scoreText);
            window.draw(hiScoreText);
        }
        
        window.display();

    } // End game loop

    
    return 0;
}

// Better collision detection using radii
bool circlesIntersect(sf::Vector2f pos1, sf::Vector2f pos2) {

    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float distance = sqrt(dx * dx + dy * dy);
    
    return distance < PLAYER_RADIUS;
}