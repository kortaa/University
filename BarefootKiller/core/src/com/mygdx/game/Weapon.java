package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.utils.Array;
import java.lang.Math;

public class Weapon extends Body{
    private Player player;
    float attackSpeed;
    float attackTimer;
    public Weapon(Player p, float attackT){
        attackSpeed = attackT;
        player = p;
        createBody(player.getX(), player.getX(), 128, 64);
        createTextureRegion("sword.png");
        setDamage(10);
    }
    public Enemy getTarget(Array<Enemy> Targets){
        double minVal = Double.MAX_VALUE;
        int minIndex = 0;
        int i = 0;
        for (Enemy enemy : Targets){
            if(enemy.isSpawning == false) {
                double temp = Math.pow(enemy.getX() - player.getX(), 2) +
                        Math.pow(enemy.getY() - player.getY(), 2);
                if (temp < minVal) {
                    minVal = temp;
                    minIndex = i;
                }
            }
            i++;
        }
        return Targets.get(minIndex);
    }
    private float calculateAngle(Enemy target){
        double angleTemp = Math.atan2(target.getY() - player.getY(), target.getX() - player.getX());
        return (float) Math.toDegrees(angleTemp);
    }
    public void update(Array<Enemy> Targets){
        setX(player.getX() + 0.65f * player.getWidth());
        setY(player.getY() - player.getHeight() / 2);
        if(Targets.notEmpty()) setAngle(calculateAngle(getTarget(Targets)));

        attackTimer += Gdx.graphics.getDeltaTime();
    }
    public boolean isShooting(){
        if (attackTimer >= attackSpeed) {
            attackTimer -= attackSpeed;
            return true;
        }
        return false;
    }
}