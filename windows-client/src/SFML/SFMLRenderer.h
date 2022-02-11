#ifndef SFML_RENDERER_H
#define SFML_RENDERER_H

#include <SFML/Graphics.hpp>

#include "pch.h"

class SFMLRenderer {
private:
    std::atomic<std::array<uint8_t, 8>> data;

    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 500;

    sf::Sprite buttonSprites[14];
    sf::Sprite dpadSprites[4];
    sf::Sprite lStickSprite;
    sf::Sprite rStickSprite;
    sf::CircleShape lStickDot;
    sf::CircleShape rStickDot;

    const sf::IntRect buttonRects[14] = {
        sf::IntRect(1620, 675, 225, 225),  // Y
        sf::IntRect(1825, 875, 225, 225),  // B
        sf::IntRect(2025, 675, 225, 225),  // A
        sf::IntRect(1825, 500, 225, 225),  // X
        sf::IntRect(200, 275, 500, 200),   // L
        sf::IntRect(1800, 275, 500, 200),  // R
        sf::IntRect(700, 275, 500, 200),   // XL
        sf::IntRect(1250, 275, 500, 200),  // XR
        sf::IntRect(800, 550, 150, 150),   // Select
        sf::IntRect(1475, 550, 150, 150),  // Start
        sf::IntRect(550, 1475, 500, 200),  // LClick
        sf::IntRect(1300, 1475, 500, 200), // RClick
        sf::IntRect(1350, 750, 150, 150),  // Home
        sf::IntRect(950, 750, 150, 150)    // Capture
    };

    const sf::IntRect dpadRects[4] = {
        sf::IntRect(719, 988, 145, 145),  // up
        sf::IntRect(851, 1117, 145, 145), // right
        sf::IntRect(719, 1249, 145, 145), // down
        sf::IntRect(589, 1117, 145, 145)  // left
    };

    const float stickRadius = 255.f;
    const sf::Vector2f lStickCenter =
        sf::Vector2f(208 + stickRadius, 520 + stickRadius);
    const sf::Vector2f rStickCenter =
        sf::Vector2f(1309 + stickRadius, 935 + stickRadius);

public:
    SFMLRenderer() {}

    void start() {

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                                "Switch Controller");
        window.setFramerateLimit(30);

        sf::Texture texture;
        texture.setSmooth(true);
        if (!texture.loadFromFile("./data/controller-image/streamer-pro.png")) {
            std::cerr << "error loading controller display file\n";
            return;
        }

        // objects.push_back(std::make_shared<sf::Sprite>(texture));

        for (int i = 0; i < 14; i++) {
            buttonSprites[i] = sf::Sprite(texture);
            buttonSprites[i].setTextureRect(buttonRects[i]);
            buttonSprites[i].setPosition(buttonRects[i].left / 4.f,
                                         buttonRects[i].top / 4.f);
            buttonSprites[i].setScale(0.25f, 0.25f);
        }

        for (int i = 0; i < 4; i++) {
            dpadSprites[i] = sf::Sprite(texture);
            dpadSprites[i].setTextureRect(dpadRects[i]);
            dpadSprites[i].setPosition(dpadRects[i].left / 4.f,
                                       dpadRects[i].top / 4.f);
            dpadSprites[i].setScale(0.25f, 0.25f);
        }
        lStickSprite = sf::Sprite(texture);
        lStickSprite.setTextureRect(sf::IntRect(
            (int)(lStickCenter.x - stickRadius + 0.5f),
            (int)(lStickCenter.y - stickRadius + 0.5f),
            (int)(stickRadius * 2 + 0.5f), (int)(stickRadius * 2 + 0.5f)));
        lStickSprite.setPosition((lStickCenter.x - stickRadius) / 4.f,
                                 (lStickCenter.y - stickRadius) / 4.f);
        lStickSprite.setScale(0.25f, 0.25f);

        rStickSprite = sf::Sprite(texture);
        rStickSprite.setTextureRect(sf::IntRect(
            (int)(rStickCenter.x - stickRadius + 0.5f),
            (int)(rStickCenter.y - stickRadius + 0.5f),
            (int)(stickRadius * 2 + 0.5f), (int)(stickRadius * 2 + 0.5f)));
        rStickSprite.setPosition((rStickCenter.x - stickRadius) / 4.f,
                                 (rStickCenter.y - stickRadius) / 4.f);
        rStickSprite.setScale(0.25f, 0.25f);

        lStickDot = sf::CircleShape(5.f);
        lStickDot.setFillColor(sf::Color::Red);
        lStickDot.setOrigin(5.f, 5.f);
        lStickDot.setPosition(lStickCenter.x / 4.f, lStickCenter.y / 4.f);

        rStickDot = sf::CircleShape(5.f);
        rStickDot.setFillColor(sf::Color::Red);
        rStickDot.setOrigin(5.f, 5.f);
        rStickDot.setPosition(rStickCenter.x / 4.f, rStickCenter.y / 4.f);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            std::array<uint8_t, 8> localData = data.load();
            for (int i = 0; i < 8; i++) {
                buttonSprites[i].setColor(sf::Color(
                    (localData[1] & (1 << i)) ? 0xffffffff : 0x999999ff));
            }
            for (int i = 0; i < 6; i++) {
                buttonSprites[i + 8].setColor(sf::Color(
                    (localData[2] & (1 << i)) ? 0xffffffff : 0x999999ff));
            }
            // for (int i = 0; i < 4; i++) {
            //     res[i + 3] = getControlStickData(i);
            // }
            lStickDot.setPosition(
                lStickCenter.x / 4.f +
                    stickRadius * (localData[3] - 128) / 128.f / 4.f,
                lStickCenter.y / 4.f +
                    stickRadius * (localData[4] - 128) / 128.f / 4.f);
            rStickDot.setPosition(
                rStickCenter.x / 4.f +
                    stickRadius * (localData[5] - 128) / 128.f / 4.f,
                rStickCenter.y / 4.f +
                    stickRadius * (localData[6] - 128) / 128.f / 4.f);

            for (auto &sprite : dpadSprites) {
                sprite.setColor(sf::Color(0xffffffff));
            }
            switch (localData[7]) {
            case 1:
                dpadSprites[1].setColor(sf::Color(0x999999ff)); // right
            case 0:
                dpadSprites[0].setColor(sf::Color(0x999999ff)); // up
                break;
            case 3:
                dpadSprites[2].setColor(sf::Color(0x999999ff)); // down
            case 2:
                dpadSprites[1].setColor(sf::Color(0x999999ff)); // right
                break;
            case 5:
                dpadSprites[3].setColor(sf::Color(0x999999ff)); // left
            case 4:
                dpadSprites[2].setColor(sf::Color(0x999999ff)); // down
                break;
            case 7:
                dpadSprites[0].setColor(sf::Color(0x999999ff)); // up
            case 6:
                dpadSprites[3].setColor(sf::Color(0x999999ff)); // left
                break;
            }

            window.clear(sf::Color::White);
            for (auto &sprite : buttonSprites) {
                window.draw(sprite);
            }
            for (auto &sprite : dpadSprites) {
                window.draw(sprite);
            }
            window.draw(lStickSprite);
            window.draw(rStickSprite);
            window.draw(lStickDot);
            window.draw(rStickDot);
            window.display();
        }
    }
    void update(std::array<uint8_t, 8> newData) { data.store(newData); }
};

#endif