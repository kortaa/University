package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Player extends Body{

    private float movementSpeed;
    private float hp;
    private int hpMax;
    private float experience;
    private TextureRegion leftTexture;
    private TextureRegion rightTexture;

    public Player(){
        createBody(getMapSizeWidth() / 2, getMapSizeHeight() / 2, 24, 64);
        createTextureRegion("left_player.png");
        leftTexture = new TextureRegion(new Texture("left_player.png"));
        rightTexture = new TextureRegion(new Texture("right_player.png"));
        movementSpeed = 240;
        hpMax = 100;
        hp = hpMax;
        experience = 0;
    }

    public void moveLeft(OrthographicCamera camera, float delta){
        setTextureRegion(leftTexture);
        if(getX() > 1) setX(getX() - delta * movementSpeed);
        camera.position.x = getX();
    }

    public void moveRight(OrthographicCamera camera, float delta){
        setTextureRegion(rightTexture);
        if(getX() + getWidth() < getMapSizeWidth() - 84) setX(getX() + delta * movementSpeed);
        camera.position.x = getX();
    }

    public void moveUp(OrthographicCamera camera, float delta){
        if(getY() + getHeight() < getMapSizeHeight() - 48)setY(getY() + delta * movementSpeed);
        camera.position.y = getY();
    }

    public void moveDown(OrthographicCamera camera, float delta){
        if(getY() > 1)setY(getY() - delta * movementSpeed);
        camera.position.y = getY();
    }

    public float getExperience(){
        return experience;
    }

    public void setExperience(float val){
        experience = val;
    }

    public float getHp(){
        return hp;
    }

    public void setHp(float val){
        hp = val;
    }

    public int getHpMax(){
        return hpMax;
    }

    public void setHpMax(int val){
        hpMax = val;
    }

    public void increaseMovementSpeed(float speed){

        this.movementSpeed += speed;

    }

    public void decreaseMovementSpeed(float speed){

        this.movementSpeed -= speed;

    }

}
