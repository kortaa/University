package com.mygdx.game;

import com.badlogic.gdx.Gdx;

public class Experience extends Item{
    public Experience(float x,  float y, String name) {
        createTextureRegion(name);
        createBody(x, y, 32, 32);
    }

    @Override
    public void addVal(Player player){
        player.setExperience(player.getExperience() + 10f);
    }
}
