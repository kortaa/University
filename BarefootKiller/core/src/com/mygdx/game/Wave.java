package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.utils.Array;
import java.util.Random;

public class Wave {
    int number;
    float timer;
    float subTimer;
    float time;
    GameMap map;

    public Wave(GameMap map){
        this.map = map;
        number = 1;
        timer = 0;
        subTimer = 3;
        time = 5;

    }
    public void update(){
        float delta = Gdx.graphics.getDeltaTime();
        timer += delta;
        subTimer += delta;

        if (timer >= time) {
            endOfWave();
            timer = 0;
        }
        if (subTimer >= 2){
            subTimer = 0;
            if(map.enemies.size < 50)
                spawnEnemies();
        }
    }
    public void spawnEnemies(){
        int a = 4 + (number * number / 16) % 25;
        int b = 8 + (number * number / 16) % 25;
        Random random = new Random();
        int n = a + random.nextInt() % b;
        for(int i = 0; i < n; i++) map.enemies.add(new Enemy());
    }
    public void endOfWave(){
        if(number < 9) number += 1;
    }
}
