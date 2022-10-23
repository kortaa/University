package com.mygdx.game;

import com.badlogic.gdx.Gdx;

import java.lang.Math;

public abstract class Item extends Body{
    public Item() {
        createTextureRegion("item.png");
        createBody(0, 0, 32, 32);
    }
    public Item(float x,  float y, String name) {
        createTextureRegion(name);
        createBody(x, y, 32, 32);
    }
    public void addVal(Player player){}

    public void update(Player player){
        double distance = Math.pow(getX() - player.getX(), 2) + Math.pow(getY() - player.getY(), 2);
        if(distance <= Math.pow(300, 2)){
            double speed = 20 + Math.pow(300 - Math.sqrt(distance), 2)/160;
            float delta = Gdx.graphics.getDeltaTime();
            double angle = Math.atan2(player.getY() - this.getY(), player.getX() - this.getX());
            double temp = Math.cos(angle) * speed * delta;

            this.setX(this.getX() + (float) temp);
            this.setY(this.getY() + (float) (Math.sin(angle) * speed * delta));
        }
    }
}
