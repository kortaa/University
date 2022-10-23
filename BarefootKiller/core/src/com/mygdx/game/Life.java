package com.mygdx.game;

public class Life extends Item{
    public Life(float x,  float y, String name) {
        createTextureRegion(name);
        createBody(x, y, 32, 32);
    }

    @Override
    public void addVal(Player player){
        player.setHp(player.getHp() + 10);
    }
}