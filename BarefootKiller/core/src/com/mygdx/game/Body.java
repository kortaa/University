package com.mygdx.game;

import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.math.Rectangle;

import java.util.Random;

public abstract class Body {

    private Rectangle body;
    private TextureRegion textureRegion;
    private float angle;
    private final float mapSizeWidth = 2280;
    private final float mapSizeHeight = 1528;
    private float damage;

    public float getX(){
        return body.x;
    }

    public void setX(float x){
        body.x = x;
    }

    public float getY(){
        return body.y;
    }

    public void setY(float y){
        body.y = y;
    }

    public float getWidth(){
        return body.width;
    }

    public void setWidth(float x){
        body.width = x;
    }

    public float getHeight(){
        return body.height;
    }

    public void setHeight(float x){
        body.height = x;
    }

    public float getDamage(){
        return damage;
    }

    public void setDamage(float damage){
        this.damage = damage;
    }

    public float getAngle(){
        return angle;
    }

    public void setAngle(float angle){
        this.angle = angle;
    }

    public float getMapSizeWidth(){
        return mapSizeWidth;
    }

    public float getMapSizeHeight(){
        return mapSizeHeight;
    }

    public void createTextureRegion(String imagePath){
        textureRegion = new TextureRegion(new Texture(imagePath));
    }

    public TextureRegion getTextureRegion(){
        return textureRegion;
    }

    public void setTextureRegion(String imagePath){
        textureRegion = new TextureRegion(new Texture(imagePath));
    }

    public void setTextureRegion(TextureRegion textureRegion){
        this.textureRegion = textureRegion;
    }

    public Rectangle getBody(){
        return body;
    }

    public void createBody(){
        body = new Rectangle();
    }

    public void createBody(float x, float y, float width, float height){
        body = new Rectangle(x, y, width, height);
    }

    public void draw(SpriteBatch batch){
        batch.begin();
        batch.draw(textureRegion, getX(), getY());
        batch.end();
    }

    public void draw(SpriteBatch batch, float x, float y){
        batch.begin();
        batch.draw(textureRegion, x, y);
        batch.end();
    }

    public void draw(SpriteBatch batch, float x, float y, float originX, float originY, float width, float height, float scaleX, float scaleY, float angle){
        batch.begin();
        batch.draw(textureRegion, x, y, originX, originY, width, height, scaleX, scaleY, angle);
        batch.end();
    }

    public float getRandomFloatBetweenAB(float a, float b){
        return a + new Random().nextFloat() * (b - a);
    }

}
